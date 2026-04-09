#include "NodeFlowManager.h"
#include "BattleManager.h"
#include "EventManager.h"
#include "RunManager.h"
#include "ShopManager.h"

ANodeFlowManager::ANodeFlowManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

bool ANodeFlowManager::StartFlow(const int32 Seed)
{
    if (!RunManager)
    {
        return false;
    }

    RunManager->StartRun(Seed);
    ActiveNode = RunManager->GetCurrentNode();
    return EnterCurrentNode();
}

bool ANodeFlowManager::CompleteBattleNode(const bool bPlayerWon)
{
    if (FlowState != ENodeFlowState::InBattle || !BattleManager || !RunManager)
    {
        return false;
    }

    BattleManager->CompleteBattle(bPlayerWon);
    if (!bPlayerWon)
    {
        RunManager->FinishRun(false);
        SetFlowState(ENodeFlowState::RunComplete);
        return true;
    }

    // wait for reward picking in ClaimBattleRewardAndAdvance
    return true;
}

bool ANodeFlowManager::ClaimBattleRewardAndAdvance(const int32 OptionIndex, const bool bSkip)
{
    if (FlowState != ENodeFlowState::InBattle || !BattleManager)
    {
        return false;
    }

    if (!BattleManager->PickRewardCard(OptionIndex, bSkip))
    {
        return false;
    }

    return AdvanceToNextNodeOrComplete();
}

bool ANodeFlowManager::ResolveEventAndAdvance(const int32 OptionIndex)
{
    if (FlowState != ENodeFlowState::InEvent || !EventManager || !RunManager)
    {
        return false;
    }

    if (!EventManager->ChooseOption(OptionIndex, RunManager))
    {
        return false;
    }

    return AdvanceToNextNodeOrComplete();
}

bool ANodeFlowManager::LeaveShopAndAdvance()
{
    if (FlowState != ENodeFlowState::InShop)
    {
        return false;
    }

    return AdvanceToNextNodeOrComplete();
}

void ANodeFlowManager::SetFlowState(const ENodeFlowState NewState)
{
    FlowState = NewState;
    OnFlowStateChanged.Broadcast(FlowState);
}

bool ANodeFlowManager::EnterCurrentNode()
{
    if (!RunManager)
    {
        return false;
    }

    ActiveNode = RunManager->GetCurrentNode();
    OnFlowNodeEntered.Broadcast(ActiveNode);
    SetFlowState(ENodeFlowState::EnteringNode);

    switch (ActiveNode.NodeType)
    {
    case EMapNodeType::Battle:
    case EMapNodeType::Elite:
    case EMapNodeType::Boss:
        if (!BattleManager)
        {
            return false;
        }
        BattleManager->StartBattle();
        SetFlowState(ENodeFlowState::InBattle);
        return true;
    case EMapNodeType::Event:
        if (!EventManager || !EventManager->RollRandomEvent())
        {
            return false;
        }
        SetFlowState(ENodeFlowState::InEvent);
        return true;
    case EMapNodeType::Shop:
        if (!ShopManager)
        {
            return false;
        }
        ShopManager->GenerateShop(RunManager);
        SetFlowState(ENodeFlowState::InShop);
        return true;
    case EMapNodeType::Rest:
        RunManager->HealPlayer(RestHealAmount);
        SetFlowState(ENodeFlowState::InRest);
        return AdvanceToNextNodeOrComplete();
    default:
        return false;
    }
}

bool ANodeFlowManager::AdvanceToNextNodeOrComplete()
{
    if (!RunManager)
    {
        return false;
    }

    if (ActiveNode.NodeType == EMapNodeType::Boss)
    {
        RunManager->FinishRun(true);
        SetFlowState(ENodeFlowState::RunComplete);
        return true;
    }

    if (!RunManager->AdvanceToNextNode())
    {
        RunManager->FinishRun(true);
        SetFlowState(ENodeFlowState::RunComplete);
        return true;
    }

    ActiveNode = RunManager->GetCurrentNode();
    return EnterCurrentNode();
}
