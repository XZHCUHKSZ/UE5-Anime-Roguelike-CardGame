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
    CardSystem->SetCardDataTable(CardDataTable);
    CardSystem->InitializeDeck(StarterDeck);
    EnterPlayerTurnStart();
}

bool ABattleManager::TryPlayCardFromHand(const int32 HandIndex)
{
    if (CurrentPhase != EBattlePhase::PlayerMain)
    {
        return false;
    }

    int32 PlayedCost = 0;
    if (!CardSystem->TryPlayCard(HandIndex, CurrentEnergy, PlayedCost))
    {
        return false;
    }

    CurrentEnergy = FMath::Max(0, CurrentEnergy - PlayedCost);
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
    CardSystem->DrawCards(DrawPerTurn);
    CurrentPhase = EBattlePhase::PlayerMain;
}

void ABattleManager::EnterEnemyTurn()
{
    CurrentPhase = EBattlePhase::EnemyTurnStart;
    CurrentPhase = EBattlePhase::EnemyMain;

    // TODO: Implement enemy intent and action logic.

    CurrentPhase = EBattlePhase::EnemyTurnEnd;
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
