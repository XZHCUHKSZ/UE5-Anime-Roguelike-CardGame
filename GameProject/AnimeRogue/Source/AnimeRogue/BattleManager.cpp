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

void ABattleManager::ConfigureEncounter(const TArray<FName>& EnemyIds)
{
    EnemyUnits.Reset();

    TArray<FName> LocalIds = EnemyIds;
    if (LocalIds.IsEmpty())
    {
        LocalIds.Add("enemy_slime_a");
    }

    for (const FName EnemyId : LocalIds)
    {
        FEnemyRuntimeState Enemy;
        Enemy.EnemyId = EnemyId;
        Enemy.State.MaxHP = EnemyStartHP;
        Enemy.State.HP = EnemyStartHP;
        Enemy.State.Block = 0;
        Enemy.IntentScriptId = EnemyIntentScriptId;
        Enemy.IntentIndex = 0;

        if (EnemyDataTable)
        {
            if (const FEnemyDataRow* Row = EnemyDataTable->FindRow<FEnemyDataRow>(EnemyId, TEXT("ConfigureEncounter")))
            {
                Enemy.State.MaxHP = FMath::Max(1, Row->HpMax);
                Enemy.State.HP = Enemy.State.MaxHP;
                if (!Row->IntentPattern.IsEmpty())
                {
                    Row->IntentPattern.ParseIntoArray(Enemy.IntentSequence, TEXT("|"), true);
                }
            }
        }

        if (Enemy.IntentSequence.IsEmpty())
        {
            LoadIntentSequenceForEnemy(Enemy);
        }

        EnemyUnits.Add(Enemy);
    }

    SyncPrimaryEnemyFromArray();
}

void ABattleManager::StartBattle()
{
    CurrentPhase = EBattlePhase::BattleStart;
    BattleLog.Reset();
    InitializeBattleStates();
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
    ProcessTurnStartStatuses(PlayerState, true);
    CardSystem->DrawCards(DrawPerTurn);
    CurrentEnemyIntent = ResolveCurrentEnemyIntent();
    SyncPrimaryEnemyToArray();
    AddBattleLog(FString::Printf(TEXT("Player turn start. Energy=%d, EnemyIntent=%s"), CurrentEnergy, *CurrentEnemyIntent));
    CurrentPhase = EBattlePhase::PlayerMain;
}

void ABattleManager::EnterEnemyTurn()
{
    CurrentPhase = EBattlePhase::EnemyTurnStart;
    EnemyState.Block = 0;
    ProcessTurnStartStatuses(EnemyState, false);
    CurrentPhase = EBattlePhase::EnemyMain;

    const FString IntentToken = ResolveCurrentEnemyIntent();
    ExecuteEnemyIntent(IntentToken);
    EnemyIntentIndex += 1;
    SyncPrimaryEnemyToArray();

    CurrentPhase = EBattlePhase::EnemyTurnEnd;

    if (PlayerState.HP <= 0)
    {
        AddBattleLog(TEXT("Player defeated."));
        CompleteBattle(false);
        return;
    }

    if (!HasAliveEnemies())
    {
        AddBattleLog(TEXT("All enemies defeated."));
        CompleteBattle(true);
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
    PlayerState.StatusStacks.Reset();

    if (EnemyUnits.IsEmpty())
    {
        ConfigureEncounter({});
    }

    SyncPrimaryEnemyFromArray();
}

void ABattleManager::LoadEnemyIntentSequence()
{
    EnemyIntentSequence = { TEXT("atk6"), TEXT("atk6"), TEXT("def5") };
    EnemyIntentIndex = 0;
}

void ABattleManager::LoadIntentSequenceForEnemy(FEnemyRuntimeState& EnemyUnit)
{
    EnemyUnit.IntentSequence.Reset();
    EnemyUnit.IntentIndex = 0;

    if (!EnemyIntentScriptTable)
    {
        EnemyUnit.IntentSequence = { TEXT("atk6"), TEXT("atk6"), TEXT("def5") };
        return;
    }

    const FName ScriptId = EnemyUnit.IntentScriptId.IsNone() ? EnemyIntentScriptId : EnemyUnit.IntentScriptId;
    const FEnemyIntentScriptRow* Row = EnemyIntentScriptTable->FindRow<FEnemyIntentScriptRow>(ScriptId, TEXT("LoadIntentSequenceForEnemy"));
    if (!Row || Row->Pattern.IsEmpty())
    {
        EnemyUnit.IntentSequence = { TEXT("atk6"), TEXT("atk6"), TEXT("def5") };
        return;
    }

    Row->Pattern.ParseIntoArray(EnemyUnit.IntentSequence, TEXT("|"), true);
    if (EnemyUnit.IntentSequence.IsEmpty())
    {
        EnemyUnit.IntentSequence = { TEXT("atk6"), TEXT("atk6"), TEXT("def5") };
    }
}

FString ABattleManager::ResolveCurrentEnemyIntent() const
{
    const int32 PrimaryIdx = GetPrimaryEnemyIndex();
    if (!EnemyUnits.IsValidIndex(PrimaryIdx))
    {
        return TEXT("atk6");
    }

    const FEnemyRuntimeState& Enemy = EnemyUnits[PrimaryIdx];
    if (Enemy.IntentSequence.IsEmpty())
    {
        return TEXT("atk6");
    }

    const int32 SafeIndex = Enemy.IntentIndex % Enemy.IntentSequence.Num();
    return Enemy.IntentSequence[SafeIndex];
}

void ABattleManager::ExecuteEnemyIntent(const FString& IntentToken)
{
    if (IntentToken.StartsWith(TEXT("atk")))
    {
        const int32 Damage = FCString::Atoi(*IntentToken.RightChop(3));
        const int32 FinalDamage = ModifyOutgoingDamage(Damage, EnemyState, false);
        ApplyDamageToPlayer(FinalDamage);
        AddBattleLog(FString::Printf(TEXT("Enemy uses %s, deals %d damage."), *IntentToken, FinalDamage));

        const int32 Thorns = GetStatusStack(PlayerState, FName(TEXT("Thorns")));
        if (Thorns > 0)
        {
            ApplyDamageToEnemy(Thorns);
            AddBattleLog(FString::Printf(TEXT("Player Thorns reflects %d damage."), Thorns));
        }
        return;
    }

    if (IntentToken.StartsWith(TEXT("def")))
    {
        const int32 BlockValue = FCString::Atoi(*IntentToken.RightChop(3));
        GainEnemyBlock(BlockValue);
        AddBattleLog(FString::Printf(TEXT("Enemy uses %s, gains %d block."), *IntentToken, BlockValue));
        return;
    }

    if (IntentToken.StartsWith(TEXT("buff_str")))
    {
        const int32 Delta = FCString::Atoi(*IntentToken.RightChop(8));
        AddStatusStack(EnemyState, FName(TEXT("Strength")), FMath::Max(1, Delta));
        SyncPrimaryEnemyToArray();
        AddBattleLog(FString::Printf(TEXT("Enemy gains %d Strength."), FMath::Max(1, Delta)));
        return;
    }

    if (IntentToken.StartsWith(TEXT("debuff_weak")))
    {
        const int32 Delta = FCString::Atoi(*IntentToken.RightChop(11));
        AddStatusStack(PlayerState, FName(TEXT("Weak")), FMath::Max(1, Delta));
        AddBattleLog(FString::Printf(TEXT("Player gains %d Weak."), FMath::Max(1, Delta)));
        return;
    }

    if (IntentToken.StartsWith(TEXT("debuff_frail")))
    {
        const int32 Delta = FCString::Atoi(*IntentToken.RightChop(12));
        AddStatusStack(PlayerState, FName(TEXT("Frail")), FMath::Max(1, Delta));
        AddBattleLog(FString::Printf(TEXT("Player gains %d Frail."), FMath::Max(1, Delta)));
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

    Damage = ModifyIncomingDamage(Damage, PlayerState);
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

    Damage = ModifyIncomingDamage(Damage, EnemyState);
    const int32 BlockAbsorb = FMath::Min(EnemyState.Block, Damage);
    EnemyState.Block -= BlockAbsorb;
    const int32 FinalDamage = Damage - BlockAbsorb;
    EnemyState.HP = FMath::Max(0, EnemyState.HP - FinalDamage);
    SyncPrimaryEnemyToArray();
}

void ABattleManager::GainPlayerBlock(const int32 BlockValue)
{
    if (BlockValue <= 0)
    {
        return;
    }

    int32 FinalBlock = BlockValue;
    if (GetStatusStack(PlayerState, FName(TEXT("Frail"))) > 0)
    {
        FinalBlock = FMath::Max(0, FMath::FloorToInt(static_cast<float>(BlockValue) * 0.75f));
    }
    PlayerState.Block += FinalBlock;
}

void ABattleManager::GainEnemyBlock(const int32 BlockValue)
{
    if (BlockValue <= 0)
    {
        return;
    }
    EnemyState.Block += BlockValue;
    SyncPrimaryEnemyToArray();
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
        const int32 FinalDamage = ModifyOutgoingDamage(Value, PlayerState, true);
        ApplyDamageToEnemy(FinalDamage);
        AddBattleLog(FString::Printf(TEXT("Play %s: deal %d damage."), *CardId.ToString(), FinalDamage));
    }
    else if (EffectId == "effect_two_hit_damage")
    {
        const int32 HitDamage = ModifyOutgoingDamage(Value, PlayerState, true);
        ApplyDamageToEnemy(HitDamage);
        ApplyDamageToEnemy(HitDamage);
        AddBattleLog(FString::Printf(TEXT("Play %s: two hits for %d each."), *CardId.ToString(), HitDamage));
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
    else if (EffectId == "effect_apply_bleed_2")
    {
        const int32 FinalDamage = ModifyOutgoingDamage(Value, PlayerState, true);
        ApplyDamageToEnemy(FinalDamage);
        AddStatusStack(EnemyState, FName(TEXT("Bleed")), 2);
        SyncPrimaryEnemyToArray();
        AddBattleLog(FString::Printf(TEXT("Play %s: deal %d and apply 2 Bleed."), *CardId.ToString(), FinalDamage));
    }
    else if (EffectId == "effect_gain_block_and_thorns")
    {
        GainPlayerBlock(Value);
        AddStatusStack(PlayerState, FName(TEXT("Thorns")), 1);
        AddBattleLog(FString::Printf(TEXT("Play %s: gain block and 1 Thorns."), *CardId.ToString()));
    }
    else if (EffectId == "effect_gain_1_energy")
    {
        CurrentEnergy += 1;
        AddBattleLog(FString::Printf(TEXT("Play %s: gain 1 energy."), *CardId.ToString()));
    }
    else if (EffectId == "effect_discard_draw")
    {
        CardSystem->DrawCards(1);
        AddBattleLog(FString::Printf(TEXT("Play %s: cycle 1 card."), *CardId.ToString()));
    }
    else if (EffectId == "effect_next_attack_plus_damage")
    {
        AddStatusStack(PlayerState, FName(TEXT("NextAttackBonus")), Value);
        AddBattleLog(FString::Printf(TEXT("Play %s: next attack +%d damage."), *CardId.ToString(), Value));
    }
    else if (EffectId == "effect_bonus_vs_block")
    {
        int32 Damage = Value;
        if (EnemyState.Block > 0)
        {
            Damage += 4;
        }
        Damage = ModifyOutgoingDamage(Damage, PlayerState, true);
        ApplyDamageToEnemy(Damage);
        AddBattleLog(FString::Printf(TEXT("Play %s: deal %d (bonus vs block)."), *CardId.ToString(), Damage));
    }
    else if (EffectId == "effect_heal_small")
    {
        HealPlayer(FMath::Max(1, Value));
        AddBattleLog(FString::Printf(TEXT("Play %s: heal %d."), *CardId.ToString(), FMath::Max(1, Value)));
    }
    else if (EffectId == "effect_signature_execute")
    {
        int32 Damage = Value;
        if (EnemyState.HP * 10 <= EnemyState.MaxHP * 4)
        {
            Damage += 10;
        }
        Damage = ModifyOutgoingDamage(Damage, PlayerState, true);
        ApplyDamageToEnemy(Damage);
        AddBattleLog(FString::Printf(TEXT("Play %s: signature hit %d."), *CardId.ToString(), Damage));
    }
    else
    {
        if (Card->CardType == ECardType::Attack)
        {
            const int32 FinalDamage = ModifyOutgoingDamage(Value, PlayerState, true);
            ApplyDamageToEnemy(FinalDamage);
            AddBattleLog(FString::Printf(TEXT("Play %s: fallback attack %d damage."), *CardId.ToString(), FinalDamage));
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
        SyncPrimaryEnemyToArray();
        if (!HasAliveEnemies())
        {
            CompleteBattle(true);
        }
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

FBattleSaveData ABattleManager::BuildBattleSaveData() const
{
    FBattleSaveData Data;
    Data.bHasActiveBattle = (CurrentPhase != EBattlePhase::BattleEnd);
    Data.CurrentPhase = CurrentPhase;
    Data.CurrentEnergy = CurrentEnergy;
    Data.PlayerState = PlayerState;
    Data.EnemyState = EnemyState;
    Data.EnemyIntentScriptId = EnemyIntentScriptId;
    Data.EnemyIntentIndex = EnemyIntentIndex;
    Data.CurrentEnemyIntent = CurrentEnemyIntent;
    if (CardSystem)
    {
        Data.CardSystem = CardSystem->BuildSaveData();
    }
    return Data;
}

void ABattleManager::ApplyBattleSaveData(const FBattleSaveData& InData)
{
    CurrentPhase = InData.CurrentPhase;
    CurrentEnergy = InData.CurrentEnergy;
    PlayerState = InData.PlayerState;
    EnemyState = InData.EnemyState;
    EnemyIntentScriptId = InData.EnemyIntentScriptId;
    EnemyIntentIndex = FMath::Max(0, InData.EnemyIntentIndex);
    LoadEnemyIntentSequence();
    CurrentEnemyIntent = InData.CurrentEnemyIntent.IsEmpty() ? ResolveCurrentEnemyIntent() : InData.CurrentEnemyIntent;
    if (CardSystem)
    {
        CardSystem->SetCardDataTable(CardDataTable);
        CardSystem->ApplySaveData(InData.CardSystem);
    }

    SyncPrimaryEnemyToArray();
    AddBattleLog(TEXT("Battle snapshot restored."));
}

int32 ABattleManager::GetStatusStack(const FUnitState& Unit, const FName StatusId) const
{
    const int32* Value = Unit.StatusStacks.Find(StatusId);
    return Value ? *Value : 0;
}

void ABattleManager::AddStatusStack(FUnitState& Unit, const FName StatusId, const int32 Delta)
{
    if (Delta == 0 || StatusId.IsNone())
    {
        return;
    }
    const int32 Current = GetStatusStack(Unit, StatusId);
    const int32 Next = FMath::Max(0, Current + Delta);
    if (Next <= 0)
    {
        Unit.StatusStacks.Remove(StatusId);
    }
    else
    {
        Unit.StatusStacks.Add(StatusId, Next);
    }
}

void ABattleManager::DecayStatus(FUnitState& Unit, const FName StatusId, const int32 DecayValue)
{
    if (DecayValue <= 0)
    {
        return;
    }
    AddStatusStack(Unit, StatusId, -DecayValue);
}

int32 ABattleManager::ModifyOutgoingDamage(int32 BaseDamage, const FUnitState& Attacker, const bool bConsumeNextAttackBonus)
{
    int32 Result = BaseDamage;
    Result += GetStatusStack(Attacker, FName(TEXT("Strength")));

    if (GetStatusStack(Attacker, FName(TEXT("Weak"))) > 0)
    {
        Result = FMath::Max(1, FMath::FloorToInt(static_cast<float>(Result) * 0.75f));
    }

    if (bConsumeNextAttackBonus)
    {
        const int32 NextBonus = GetStatusStack(PlayerState, FName(TEXT("NextAttackBonus")));
        if (NextBonus > 0)
        {
            Result += NextBonus;
            PlayerState.StatusStacks.Remove(FName(TEXT("NextAttackBonus")));
        }
    }

    return FMath::Max(0, Result);
}

int32 ABattleManager::ModifyIncomingDamage(int32 Damage, const FUnitState& Defender) const
{
    int32 Result = Damage;
    if (GetStatusStack(Defender, FName(TEXT("Vulnerable"))) > 0)
    {
        Result = FMath::FloorToInt(static_cast<float>(Result) * 1.5f);
    }
    return FMath::Max(0, Result);
}

void ABattleManager::ProcessTurnStartStatuses(FUnitState& Unit, const bool bIsPlayer)
{
    const int32 Bleed = GetStatusStack(Unit, FName(TEXT("Bleed")));
    if (Bleed > 0)
    {
        if (bIsPlayer)
        {
            ApplyDamageToPlayer(Bleed);
        }
        else
        {
            ApplyDamageToEnemy(Bleed);
        }
        DecayStatus(Unit, FName(TEXT("Bleed")), 1);
        AddBattleLog(FString::Printf(TEXT("%s suffers %d Bleed damage."), bIsPlayer ? TEXT("Player") : TEXT("Enemy"), Bleed));
    }

    DecayStatus(Unit, FName(TEXT("Weak")), 1);
    DecayStatus(Unit, FName(TEXT("Vulnerable")), 1);
    DecayStatus(Unit, FName(TEXT("Frail")), 1);
}

void ABattleManager::HealPlayer(const int32 Value)
{
    if (Value <= 0)
    {
        return;
    }
    PlayerState.HP = FMath::Min(PlayerState.MaxHP, PlayerState.HP + Value);
}

int32 ABattleManager::GetPrimaryEnemyIndex() const
{
    for (int32 i = 0; i < EnemyUnits.Num(); ++i)
    {
        if (EnemyUnits[i].State.HP > 0)
        {
            return i;
        }
    }
    return 0;
}

bool ABattleManager::HasAliveEnemies() const
{
    for (const FEnemyRuntimeState& Enemy : EnemyUnits)
    {
        if (Enemy.State.HP > 0)
        {
            return true;
        }
    }
    return false;
}

void ABattleManager::SyncPrimaryEnemyToArray()
{
    const int32 PrimaryIdx = GetPrimaryEnemyIndex();
    if (!EnemyUnits.IsValidIndex(PrimaryIdx))
    {
        return;
    }

    EnemyUnits[PrimaryIdx].State = EnemyState;
    EnemyUnits[PrimaryIdx].CurrentIntent = CurrentEnemyIntent;
    EnemyUnits[PrimaryIdx].IntentIndex = EnemyIntentIndex;
}

void ABattleManager::SyncPrimaryEnemyFromArray()
{
    const int32 PrimaryIdx = GetPrimaryEnemyIndex();
    if (!EnemyUnits.IsValidIndex(PrimaryIdx))
    {
        return;
    }

    EnemyState = EnemyUnits[PrimaryIdx].State;
    EnemyIntentScriptId = EnemyUnits[PrimaryIdx].IntentScriptId;
    EnemyIntentIndex = EnemyUnits[PrimaryIdx].IntentIndex;
    EnemyIntentSequence = EnemyUnits[PrimaryIdx].IntentSequence;
    CurrentEnemyIntent = EnemyUnits[PrimaryIdx].CurrentIntent;
}
