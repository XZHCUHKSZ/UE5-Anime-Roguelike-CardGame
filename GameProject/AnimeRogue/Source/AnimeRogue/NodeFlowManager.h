#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueTypes.h"
#include "NodeFlowManager.generated.h"

class ARunManager;
class ABattleManager;
class AEventManager;
class AShopManager;
class ASaveManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlowStateChanged, ENodeFlowState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlowNodeEntered, const FMapNodeData&, NodeData);

UCLASS()
class ANIMEROGUE_API ANodeFlowManager : public AActor
{
    GENERATED_BODY()

public:
    ANodeFlowManager();

    UFUNCTION(BlueprintCallable)
    bool StartFlow(int32 Seed);

    UFUNCTION(BlueprintCallable)
    bool StartFlowFromSave();

    UFUNCTION(BlueprintCallable)
    bool RestoreFromLoadedData(ENodeFlowState SavedFlowState, const FBattleSaveData& BattleSaveData);

    UFUNCTION(BlueprintCallable)
    bool CompleteBattleNode(bool bPlayerWon);

    UFUNCTION(BlueprintCallable)
    bool ClaimBattleRewardAndAdvance(int32 OptionIndex, bool bSkip);

    UFUNCTION(BlueprintCallable)
    bool ResolveEventAndAdvance(int32 OptionIndex);

    UFUNCTION(BlueprintCallable)
    bool LeaveShopAndAdvance();

    UFUNCTION(BlueprintPure)
    ENodeFlowState GetFlowState() const { return FlowState; }

    UFUNCTION(BlueprintPure)
    FMapNodeData GetActiveNode() const { return ActiveNode; }

    UPROPERTY(BlueprintAssignable)
    FOnFlowStateChanged OnFlowStateChanged;

    UPROPERTY(BlueprintAssignable)
    FOnFlowNodeEntered OnFlowNodeEntered;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flow")
    ARunManager* RunManager = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flow")
    ABattleManager* BattleManager = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flow")
    AEventManager* EventManager = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flow")
    AShopManager* ShopManager = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flow")
    ASaveManager* SaveManager = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flow")
    int32 RestHealAmount = 18;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flow")
    bool bEnableAutoSave = true;

private:
    UPROPERTY()
    ENodeFlowState FlowState = ENodeFlowState::Idle;

    UPROPERTY()
    FMapNodeData ActiveNode;

    void SetFlowState(ENodeFlowState NewState);
    bool EnterCurrentNode();
    bool AdvanceToNextNodeOrComplete();
    void AutoSaveIfEnabled();
};
