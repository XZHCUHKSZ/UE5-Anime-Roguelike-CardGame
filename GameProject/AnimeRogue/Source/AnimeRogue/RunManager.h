#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueTypes.h"
#include "RunManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNodeAdvanced, const FMapNodeData&, NewNode);

UCLASS()
class ANIMEROGUE_API ARunManager : public AActor
{
    GENERATED_BODY()

public:
    ARunManager();

    UFUNCTION(BlueprintCallable)
    void StartRun(int32 Seed);

    UFUNCTION(BlueprintCallable)
    bool AdvanceToNextNode();

    UFUNCTION(BlueprintPure)
    const FMapNodeData& GetCurrentNode() const { return CurrentNode; }

    UFUNCTION(BlueprintPure)
    int32 GetRunSeed() const { return RunSeed; }

    UPROPERTY(BlueprintAssignable)
    FOnNodeAdvanced OnNodeAdvanced;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Run")
    int32 MaxFloors = 12;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Run")
    int32 RunSeed = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Run")
    FMapNodeData CurrentNode;

private:
    EMapNodeType ResolveNodeTypeByFloor(int32 Floor) const;
};
