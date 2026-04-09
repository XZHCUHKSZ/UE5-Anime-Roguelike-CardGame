#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RogueTypes.h"
#include "RogueRunSaveGame.generated.h"

UCLASS()
class ANIMEROGUE_API URogueRunSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    FRunSaveData RunData;
};
