#include "RogueUIBridgeLibrary.h"
#include "BattleManager.h"
#include "CardSystemComponent.h"
#include "EventManager.h"
#include "NodeFlowManager.h"
#include "RunManager.h"
#include "ShopManager.h"

FUIRuntimeSnapshot URogueUIBridgeLibrary::BuildUIRuntimeSnapshot(
    ANodeFlowManager* NodeFlowManager,
    ARunManager* RunManager,
    ABattleManager* BattleManager,
    AEventManager* EventManager,
    AShopManager* ShopManager
)
{
    FUIRuntimeSnapshot Snapshot;

    if (NodeFlowManager)
    {
        Snapshot.FlowState = NodeFlowManager->GetFlowState();
        Snapshot.Run.ActiveNode = NodeFlowManager->GetActiveNode();
    }

    if (RunManager)
    {
        Snapshot.Run.bRunActive = RunManager->IsRunActive();
        Snapshot.Run.Gold = RunManager->GetGold();
        Snapshot.Run.CurrentHP = RunManager->GetCurrentHP();
        Snapshot.Run.MaxHP = RunManager->GetMaxHP();
        Snapshot.Run.RunLog = RunManager->GetRunLog();

        if (!NodeFlowManager)
        {
            Snapshot.Run.ActiveNode = RunManager->GetCurrentNode();
        }
    }

    if (BattleManager)
    {
        Snapshot.Battle.Energy = BattleManager->GetCurrentEnergy();
        Snapshot.Battle.PlayerState = BattleManager->GetPlayerState();
        Snapshot.Battle.EnemyState = BattleManager->GetEnemyState();
        Snapshot.Battle.EnemyIntent = BattleManager->GetCurrentEnemyIntent();
        Snapshot.Battle.RewardOptions = BattleManager->GetCurrentRewardOptions();
        Snapshot.BattleLog = BattleManager->GetBattleLog();

        if (UCardSystemComponent* CardSystem = BattleManager->GetCardSystem())
        {
            Snapshot.Battle.HandCards = CardSystem->GetHand();
        }
    }

    if (EventManager)
    {
        const FResolvedEventData EventData = EventManager->GetCurrentEvent();
        Snapshot.Event.EventId = EventData.EventId;
        Snapshot.Event.Title = EventData.Title;
        Snapshot.Event.Options = EventData.Options;
    }

    if (ShopManager)
    {
        Snapshot.Shop.CardOffers = ShopManager->GetCurrentOffers();
    }

    return Snapshot;
}

FText URogueUIBridgeLibrary::ToNodeTypeText(const EMapNodeType NodeType)
{
    switch (NodeType)
    {
    case EMapNodeType::Battle:
        return FText::FromString(TEXT("Battle"));
    case EMapNodeType::Event:
        return FText::FromString(TEXT("Event"));
    case EMapNodeType::Shop:
        return FText::FromString(TEXT("Shop"));
    case EMapNodeType::Rest:
        return FText::FromString(TEXT("Rest"));
    case EMapNodeType::Elite:
        return FText::FromString(TEXT("Elite"));
    case EMapNodeType::Boss:
        return FText::FromString(TEXT("Boss"));
    default:
        return FText::FromString(TEXT("Unknown"));
    }
}

FText URogueUIBridgeLibrary::ToFlowStateText(const ENodeFlowState FlowState)
{
    switch (FlowState)
    {
    case ENodeFlowState::Idle:
        return FText::FromString(TEXT("Idle"));
    case ENodeFlowState::EnteringNode:
        return FText::FromString(TEXT("Entering Node"));
    case ENodeFlowState::InBattle:
        return FText::FromString(TEXT("In Battle"));
    case ENodeFlowState::InEvent:
        return FText::FromString(TEXT("In Event"));
    case ENodeFlowState::InShop:
        return FText::FromString(TEXT("In Shop"));
    case ENodeFlowState::InRest:
        return FText::FromString(TEXT("In Rest"));
    case ENodeFlowState::RunComplete:
        return FText::FromString(TEXT("Run Complete"));
    default:
        return FText::FromString(TEXT("Unknown"));
    }
}
