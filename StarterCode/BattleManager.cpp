#include "BattleManager.h"
#include "CardSystemComponent.h"

ABattleManager::ABattleManager()
{
    PrimaryActorTick.bCanEverTick = false;
    CardSystem = CreateDefaultSubobject<UCardSystemComponent>(TEXT("CardSystem"));
}

void ABattleManager::StartBattle()
{
    CurrentPhase = EBattlePhase::BattleStart;
    EnterPlayerTurnStart();
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

void ABattleManager::ConsumePlayerEnergy(const int32 Cost)
{
    CurrentEnergy = FMath::Max(0, CurrentEnergy - Cost);
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
