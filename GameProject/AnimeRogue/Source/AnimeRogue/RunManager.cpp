#include "RunManager.h"

ARunManager::ARunManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ARunManager::StartRun(const int32 Seed)
{
    RunSeed = Seed > 0 ? Seed : FMath::Rand();
    CurrentNode = {};
    CurrentNode.Floor = 1;
    CurrentNode.NodeIndex = 0;
    CurrentNode.NodeType = ResolveNodeTypeByFloor(CurrentNode.Floor);
    OnNodeAdvanced.Broadcast(CurrentNode);
}

bool ARunManager::AdvanceToNextNode()
{
    if (CurrentNode.Floor >= MaxFloors)
    {
        return false;
    }

    CurrentNode.Floor += 1;
    CurrentNode.NodeIndex += 1;
    CurrentNode.NodeType = ResolveNodeTypeByFloor(CurrentNode.Floor);
    OnNodeAdvanced.Broadcast(CurrentNode);
    return true;
}

EMapNodeType ARunManager::ResolveNodeTypeByFloor(const int32 Floor) const
{
    if (Floor >= MaxFloors)
    {
        return EMapNodeType::Boss;
    }

    if (Floor == 4 || Floor == 8)
    {
        return EMapNodeType::Elite;
    }

    if (Floor == 3 || Floor == 7 || Floor == 10)
    {
        return EMapNodeType::Event;
    }

    if (Floor == 5 || Floor == 9)
    {
        return EMapNodeType::Shop;
    }

    if (Floor == 6 || Floor == 11)
    {
        return EMapNodeType::Rest;
    }

    return EMapNodeType::Battle;
}
