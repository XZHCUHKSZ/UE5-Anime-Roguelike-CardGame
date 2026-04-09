#include "BattleManager.h"
#include "CardSystemComponent.h"
#include "Engine/DataTable.h"

ABattleManager::ABattleManager()
{
    PrimaryActorTick.bCanEverTick = false;
    CardSystem = CreateDefaultSubobject<UCardSystemComponent>(TEXT("CardSystem"));

    StarterDeck = {
        "card_strike_01",
        "card_strike_01",
        "card_strike_01",
        "card_strike_01",
        "card_strike_01",
        "card_guard_01",
        "card_guard_01",
        "card_guard_01",
        "card_guard_01",
        "card_focus_01"
    };
}

void ABattleManager::StartBattle()
{
    CurrentPhase = EBattlePhase::BattleStart;
    BattleLog.Reset();
    InitializeBattleStates();
    LoadEnemyIntentSequence();
    CardSystem->SetCardDataTable(CardDataTable);
    CardSystem->InitializeDeck(StarterDeck);
    AddBattleLog(TEXT("Battle started."));
    EnterPlayerTurnStart();
}

bool ABattleManager::TryPlayCardFromHand(const int32 HandIndex)
{
    if (CurrentPhase != EBattlePhase::PlayerMain)
    {
        return false;
    }

    const TArray<FRuntimeCard> CurrentHand = CardSystem->GetHand();
    if (!CurrentHand.IsValidIndex(HandIndex))
    {
        return false;
    }
    const FName PlayedCardId = CurrentHand[HandIndex].CardId;

    int32 PlayedCost = 0;
    if (!CardSystem->TryPlayCard(HandIndex, CurrentEnergy, PlayedCost))
    {
        return false;
    }

    CurrentEnergy = FMath::Max(0, CurrentEnergy - PlayedCost);
    ApplyCardEffectById(PlayedCardId);
    return true;
}

void ABattleManager::EndPlayerTurn()
{
    if (CurrentPhase != EBattlePhase::PlayerMain)
    {
        return;
    }

    CurrentPhase = EBattlePhase::PlayerTurnEnd;
    CardSystem->DiscardHand();
    EnterEnemyTurn();
}

void ABattleManager::CompleteBattle(const bool bPlayerWon)
{
    CurrentPhase = EBattlePhase::BattleEnd;
    if (!bPlayerWon)
    {
        CurrentRewardOptions.Reset();
        return;
    }

    GenerateCardRewards(3);
}

bool ABattleManager::PickRewardCard(const int32 OptionIndex, const bool bSkip)
{
    if (bSkip)
    {
        CurrentRewardOptions.Reset();
        return true;
    }

    if (!CurrentRewardOptions.IsValidIndex(OptionIndex))
    {
        return false;
    }

    StarterDeck.Add(CurrentRewardOptions[OptionIndex].CardId);
    CurrentRewardOptions.Reset();
    return true;
}

void ABattleManager::EnterPlayerTurnStart()
{
    CurrentPhase = EBattlePhase::PlayerTurnStart;
    CurrentEnergy = EnergyPerTurn;
    PlayerState.Block = 0;
    CardSystem->DrawCards(DrawPerTurn);
    CurrentEnemyIntent = ResolveCurrentEnemyIntent();
    AddBattleLog(FString::Printf(TEXT("Player turn start. Energy=%d, EnemyIntent=%s"), CurrentEnergy, *CurrentEnemyIntent));
    CurrentPhase = EBattlePhase::PlayerMain;
}

void ABattleManager::EnterEnemyTurn()
{
    CurrentPhase = EBattlePhase::EnemyTurnStart;
    CurrentPhase = EBattlePhase::EnemyMain;

    const FString IntentToken = ResolveCurrentEnemyIntent();
    ExecuteEnemyIntent(IntentToken);
    EnemyIntentIndex += 1;

    CurrentPhase = EBattlePhase::EnemyTurnEnd;

    if (PlayerState.HP <= 0)
    {
        AddBattleLog(TEXT("Player defeated."));
        CompleteBattle(false);
        return;
    }

    EnterPlayerTurnStart();
}

void ABattleManager::GenerateCardRewards(const int32 RewardCount)
{
    CurrentRewardOptions.Reset();
    if (RewardCardPool.IsEmpty())
    {
        RewardCardPool = {
            "card_strike_01",
            "card_guard_01",
            "card_focus_01",
            "swg_quick_01",
            "swg_finisher_01",
            "swg_bleed_01"
        };
    }

    TArray<FName> LocalPool = RewardCardPool;
    for (int32 i = LocalPool.Num() - 1; i > 0; --i)
    {
        const int32 SwapIndex = FMath::RandRange(0, i);
        LocalPool.Swap(i, SwapIndex);
    }

    const int32 PickCount = FMath::Min(RewardCount, LocalPool.Num());
    for (int32 i = 0; i < PickCount; ++i)
    {
        FCardRewardOption Option;
        Option.CardId = LocalPool[i];
        CurrentRewardOptions.Add(Option);
    }

    OnBattleRewardReady.Broadcast(CurrentRewardOptions);
}

void ABattleManager::InitializeBattleStates()
{
    PlayerState.MaxHP = PlayerStartHP;
    PlayerState.HP = PlayerStartHP;
    PlayerState.Block = 0;

    EnemyState.MaxHP = EnemyStartHP;
    EnemyState.HP = EnemyStartHP;
    EnemyState.Block = 0;
}

void ABattleManager::LoadEnemyIntentSequence()
{
    EnemyIntentSequence.Reset();
    EnemyIntentIndex = 0;

    if (!EnemyIntentScriptTable)
    {
        EnemyIntentSequence = { TEXT("atk6"), TEXT("atk6"), TEXT("def5") };
        return;
    }

    const FEnemyIntentScriptRow* Row = EnemyIntentScriptTable->FindRow<FEnemyIntentScriptRow>(
        EnemyIntentScriptId,
        TEXT("LoadEnemyIntentSequence")
    );
    if (!Row || Row->Pattern.IsEmpty())
    {
        EnemyIntentSequence = { TEXT("atk6"), TEXT("atk6"), TEXT("def5") };
        return;
    }

    Row->Pattern.ParseIntoArray(EnemyIntentSequence, TEXT("|"), true);
    if (EnemyIntentSequence.IsEmpty())
    {
        EnemyIntentSequence = { TEXT("atk6"), TEXT("atk6"), TEXT("def5") };
    }
}

FString ABattleManager::ResolveCurrentEnemyIntent() const
{
    if (EnemyIntentSequence.IsEmpty())
    {
        return TEXT("atk6");
    }

    const int32 SafeIndex = EnemyIntentIndex % EnemyIntentSequence.Num();
    return EnemyIntentSequence[SafeIndex];
}

void ABattleManager::ExecuteEnemyIntent(const FString& IntentToken)
{
    if (IntentToken.StartsWith(TEXT("atk")))
    {
        const int32 Damage = FCString::Atoi(*IntentToken.RightChop(3));
        ApplyDamageToPlayer(Damage);
        AddBattleLog(FString::Printf(TEXT("Enemy uses %s, deals %d damage."), *IntentToken, Damage));
        return;
    }

    if (IntentToken.StartsWith(TEXT("def")))
    {
        const int32 BlockValue = FCString::Atoi(*IntentToken.RightChop(3));
        EnemyState.Block += BlockValue;
        AddBattleLog(FString::Printf(TEXT("Enemy uses %s, gains %d block."), *IntentToken, BlockValue));
        return;
    }

    AddBattleLog(FString::Printf(TEXT("Enemy uses %s (not implemented)."), *IntentToken));
}

void ABattleManager::AddBattleLog(const FString& Line)
{
    BattleLog.Add(Line);
    if (BattleLog.Num() > 50)
    {
        BattleLog.RemoveAt(0, BattleLog.Num() - 50);
    }
}

void ABattleManager::ApplyDamageToPlayer(int32 Damage)
{
    if (Damage <= 0)
    {
        return;
    }

    const int32 BlockAbsorb = FMath::Min(PlayerState.Block, Damage);
    PlayerState.Block -= BlockAbsorb;
    const int32 FinalDamage = Damage - BlockAbsorb;
    PlayerState.HP = FMath::Max(0, PlayerState.HP - FinalDamage);
}

void ABattleManager::ApplyDamageToEnemy(int32 Damage)
{
    if (Damage <= 0)
    {
        return;
    }

    const int32 BlockAbsorb = FMath::Min(EnemyState.Block, Damage);
    EnemyState.Block -= BlockAbsorb;
    const int32 FinalDamage = Damage - BlockAbsorb;
    EnemyState.HP = FMath::Max(0, EnemyState.HP - FinalDamage);
}

void ABattleManager::GainPlayerBlock(const int32 BlockValue)
{
    if (BlockValue <= 0)
    {
        return;
    }

    PlayerState.Block += BlockValue;
}

void ABattleManager::ApplyCardEffectById(const FName& CardId)
{
    const FCardData* Card = FindCardData(CardId);
    if (!Card)
    {
        AddBattleLog(FString::Printf(TEXT("Card data missing: %s"), *CardId.ToString()));
        return;
    }

    const FString EffectId = Card->EffectScriptId.ToString();
    const int32 Value = FMath::Max(0, Card->BaseValue);

    if (EffectId == "effect_deal_damage" || EffectId == "effect_damage_if_played_after_attack" || EffectId == "effect_bonus_per_attack_this_turn")
    {
        ApplyDamageToEnemy(Value);
        AddBattleLog(FString::Printf(TEXT("Play %s: deal %d damage."), *CardId.ToString(), Value));
    }
    else if (EffectId == "effect_gain_block")
    {
        GainPlayerBlock(Value);
        AddBattleLog(FString::Printf(TEXT("Play %s: gain %d block."), *CardId.ToString(), Value));
    }
    else if (EffectId == "effect_draw_card")
    {
        CardSystem->DrawCards(FMath::Max(1, Value));
        AddBattleLog(FString::Printf(TEXT("Play %s: draw %d card(s)."), *CardId.ToString(), FMath::Max(1, Value)));
    }
    else
    {
        // Fallback behavior keeps prototype playable while effect scripts are still expanding.
        if (Card->CardType == ECardType::Attack)
        {
            ApplyDamageToEnemy(Value);
            AddBattleLog(FString::Printf(TEXT("Play %s: fallback attack %d damage."), *CardId.ToString(), Value));
        }
        else if (Card->CardType == ECardType::Skill)
        {
            GainPlayerBlock(Value);
            AddBattleLog(FString::Printf(TEXT("Play %s: fallback skill gain %d block."), *CardId.ToString(), Value));
        }
        else
        {
            AddBattleLog(FString::Printf(TEXT("Play %s: effect pending (%s)."), *CardId.ToString(), *EffectId));
        }
    }

    if (EnemyState.HP <= 0)
    {
        AddBattleLog(TEXT("Enemy defeated."));
        CompleteBattle(true);
    }
}

const FCardData* ABattleManager::FindCardData(const FName& CardId) const
{
    if (!CardDataTable || CardId.IsNone())
    {
        return nullptr;
    }

    return CardDataTable->FindRow<FCardData>(CardId, TEXT("FindCardData"));
}
