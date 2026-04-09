#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RogueTypes.h"
#include "RogueMetaSaveGame.generated.h"

UCLASS()
class ANIMEROGUE_API URogueMetaSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save")
    FMetaSaveData MetaData;
};
