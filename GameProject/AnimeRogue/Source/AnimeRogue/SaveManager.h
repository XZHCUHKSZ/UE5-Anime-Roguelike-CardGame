#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueTypes.h"
#include "SaveManager.generated.h"

class ARunManager;

UCLASS()
class ANIMEROGUE_API ASaveManager : public AActor
{
    GENERATED_BODY()

public:
    ASaveManager();

    UFUNCTION(BlueprintCallable)
    bool SaveRun(ARunManager* RunManager);

    UFUNCTION(BlueprintCallable)
    bool LoadRun(ARunManager* RunManager);

    UFUNCTION(BlueprintCallable)
    bool SaveMeta(const FMetaSaveData& InMetaData);

    UFUNCTION(BlueprintCallable)
    bool LoadMeta(FMetaSaveData& OutMetaData);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Save")
    FString RunSlotName = "Slot_Run_0";

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Save")
    int32 RunSlotUserIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Save")
    FString MetaSlotName = "Slot_Meta_0";

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Save")
    int32 MetaSlotUserIndex = 0;
};
