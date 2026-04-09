#include "BattleManager.h"
#include "CardSystemComponent.h"

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

    // TODO: 实装敌方意图与执行逻辑

    CurrentPhase = EBattlePhase::EnemyTurnEnd;
    EnterPlayerTurnStart();
}
