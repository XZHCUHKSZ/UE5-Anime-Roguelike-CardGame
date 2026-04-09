#include "SaveManager.h"
#include "BattleManager.h"
#include "Kismet/GameplayStatics.h"
#include "NodeFlowManager.h"
#include "RogueMetaSaveGame.h"
#include "RogueRunSaveGame.h"
#include "RunManager.h"

ASaveManager::ASaveManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

bool ASaveManager::SaveRun(ARunManager* RunManager)
{
    if (!RunManager)
    {
        return false;
    }

    URogueRunSaveGame* SaveObj = Cast<URogueRunSaveGame>(
        UGameplayStatics::CreateSaveGameObject(URogueRunSaveGame::StaticClass())
    );
    if (!SaveObj)
    {
        return false;
    }

    SaveObj->RunData = RunManager->BuildRunSaveData();
    return UGameplayStatics::SaveGameToSlot(SaveObj, RunSlotName, RunSlotUserIndex);
}

bool ASaveManager::LoadRun(ARunManager* RunManager)
{
    if (!RunManager)
    {
        return false;
    }

    if (!UGameplayStatics::DoesSaveGameExist(RunSlotName, RunSlotUserIndex))
    {
        return false;
    }

    URogueRunSaveGame* SaveObj = Cast<URogueRunSaveGame>(
        UGameplayStatics::LoadGameFromSlot(RunSlotName, RunSlotUserIndex)
    );
    if (!SaveObj)
    {
        return false;
    }

    RunManager->ApplyRunSaveData(SaveObj->RunData);
    return true;
}

bool ASaveManager::SaveMeta(const FMetaSaveData& InMetaData)
{
    URogueMetaSaveGame* SaveObj = Cast<URogueMetaSaveGame>(
        UGameplayStatics::CreateSaveGameObject(URogueMetaSaveGame::StaticClass())
    );
    if (!SaveObj)
    {
        return false;
    }

    SaveObj->MetaData = InMetaData;
    return UGameplayStatics::SaveGameToSlot(SaveObj, MetaSlotName, MetaSlotUserIndex);
}

bool ASaveManager::LoadMeta(FMetaSaveData& OutMetaData)
{
    if (!UGameplayStatics::DoesSaveGameExist(MetaSlotName, MetaSlotUserIndex))
    {
        return false;
    }

    URogueMetaSaveGame* SaveObj = Cast<URogueMetaSaveGame>(
        UGameplayStatics::LoadGameFromSlot(MetaSlotName, MetaSlotUserIndex)
    );
    if (!SaveObj)
    {
        return false;
    }

    OutMetaData = SaveObj->MetaData;
    return true;
}

bool ASaveManager::HasRunSave() const
{
    return UGameplayStatics::DoesSaveGameExist(RunSlotName, RunSlotUserIndex);
}

bool ASaveManager::ClearRunSave()
{
    if (!UGameplayStatics::DoesSaveGameExist(RunSlotName, RunSlotUserIndex))
    {
        return true;
    }

    return UGameplayStatics::DeleteGameInSlot(RunSlotName, RunSlotUserIndex);
}

bool ASaveManager::SaveFlowState(ARunManager* RunManager, ABattleManager* BattleManager, ANodeFlowManager* NodeFlowManager)
{
    if (!RunManager)
    {
        return false;
    }

    URogueRunSaveGame* SaveObj = Cast<URogueRunSaveGame>(
        UGameplayStatics::CreateSaveGameObject(URogueRunSaveGame::StaticClass())
    );
    if (!SaveObj)
    {
        return false;
    }

    SaveObj->RunData = RunManager->BuildRunSaveData();
    if (NodeFlowManager)
    {
        SaveObj->RunData.SavedFlowState = NodeFlowManager->GetFlowState();
    }

    if (BattleManager)
    {
        const FBattleSaveData BattleData = BattleManager->BuildBattleSaveData();
        SaveObj->RunData.bHasBattleSnapshot = BattleData.bHasActiveBattle;
        SaveObj->RunData.BattlePhase = BattleData.CurrentPhase;
        SaveObj->RunData.BattleEnergy = BattleData.CurrentEnergy;
        SaveObj->RunData.BattlePlayerState = BattleData.PlayerState;
        SaveObj->RunData.BattleEnemyState = BattleData.EnemyState;
        SaveObj->RunData.BattleEnemyIntentScriptId = BattleData.EnemyIntentScriptId;
        SaveObj->RunData.BattleEnemyIntentIndex = BattleData.EnemyIntentIndex;
        SaveObj->RunData.BattleCurrentEnemyIntent = BattleData.CurrentEnemyIntent;
        SaveObj->RunData.BattleDrawPile = BattleData.CardSystem.DrawPile;
        SaveObj->RunData.BattleHand = BattleData.CardSystem.Hand;
        SaveObj->RunData.BattleDiscardPile = BattleData.CardSystem.DiscardPile;
    }
    else
    {
        SaveObj->RunData.bHasBattleSnapshot = false;
    }

    return UGameplayStatics::SaveGameToSlot(SaveObj, RunSlotName, RunSlotUserIndex);
}

bool ASaveManager::LoadFlowState(ARunManager* RunManager, ABattleManager* BattleManager, ANodeFlowManager* NodeFlowManager)
{
    if (!RunManager || !NodeFlowManager)
    {
        return false;
    }

    if (!UGameplayStatics::DoesSaveGameExist(RunSlotName, RunSlotUserIndex))
    {
        return false;
    }

    URogueRunSaveGame* SaveObj = Cast<URogueRunSaveGame>(
        UGameplayStatics::LoadGameFromSlot(RunSlotName, RunSlotUserIndex)
    );
    if (!SaveObj)
    {
        return false;
    }

    RunManager->ApplyRunSaveData(SaveObj->RunData);

    FBattleSaveData BattleData;
    if (SaveObj->RunData.bHasBattleSnapshot)
    {
        BattleData.bHasActiveBattle = true;
        BattleData.CurrentPhase = SaveObj->RunData.BattlePhase;
        BattleData.CurrentEnergy = SaveObj->RunData.BattleEnergy;
        BattleData.PlayerState = SaveObj->RunData.BattlePlayerState;
        BattleData.EnemyState = SaveObj->RunData.BattleEnemyState;
        BattleData.EnemyIntentScriptId = SaveObj->RunData.BattleEnemyIntentScriptId;
        BattleData.EnemyIntentIndex = SaveObj->RunData.BattleEnemyIntentIndex;
        BattleData.CurrentEnemyIntent = SaveObj->RunData.BattleCurrentEnemyIntent;
        BattleData.CardSystem.DrawPile = SaveObj->RunData.BattleDrawPile;
        BattleData.CardSystem.Hand = SaveObj->RunData.BattleHand;
        BattleData.CardSystem.DiscardPile = SaveObj->RunData.BattleDiscardPile;
    }

    return NodeFlowManager->RestoreFromLoadedData(SaveObj->RunData.SavedFlowState, BattleData);
}
