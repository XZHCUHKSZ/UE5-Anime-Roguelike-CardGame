#include "SaveManager.h"
#include "Kismet/GameplayStatics.h"
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
