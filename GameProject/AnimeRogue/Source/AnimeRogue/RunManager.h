#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueTypes.h"
#include "RunManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNodeAdvanced, const FMapNodeData&, NewNode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunLogUpdated, const FString&, NewLine);

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
    FMapNodeData GetCurrentNode() const { return CurrentNode; }

    UFUNCTION(BlueprintPure)
    int32 GetRunSeed() const { return RunSeed; }

    UFUNCTION(BlueprintPure)
    bool IsRunActive() const { return bRunActive; }

    UFUNCTION(BlueprintPure)
    int32 GetGold() const { return Gold; }

    UFUNCTION(BlueprintPure)
    int32 GetCurrentHP() const { return CurrentHP; }

    UFUNCTION(BlueprintPure)
    int32 GetMaxHP() const { return MaxHP; }

    UFUNCTION(BlueprintPure)
    TArray<FName> GetDeck() const { return DeckCardIds; }

    UFUNCTION(BlueprintPure)
    TArray<FName> GetRelics() const { return RelicIds; }

    UFUNCTION(BlueprintPure)
    TArray<FString> GetRunLog() const { return RunLog; }

    UFUNCTION(BlueprintCallable)
    void GainGold(int32 Amount);

    UFUNCTION(BlueprintCallable)
    bool SpendGold(int32 Amount);

    UFUNCTION(BlueprintCallable)
    void HealPlayer(int32 Amount);

    UFUNCTION(BlueprintCallable)
    void DamagePlayer(int32 Amount);

    UFUNCTION(BlueprintCallable)
    void AddCardToDeck(FName CardId);

    UFUNCTION(BlueprintCallable)
    bool RemoveCardAt(int32 DeckIndex);

    UFUNCTION(BlueprintCallable)
    bool BuyCard(FName CardId, int32 Price);

    UFUNCTION(BlueprintCallable)
    bool BuyRandomRelic(int32 Price);

    UFUNCTION(BlueprintCallable)
    bool RemoveCardAtShop(int32 DeckIndex);

    UFUNCTION(BlueprintCallable)
    bool ExecuteEventScript(FName EventScriptId);

    UPROPERTY(BlueprintAssignable)
    FOnNodeAdvanced OnNodeAdvanced;

    UPROPERTY(BlueprintAssignable)
    FOnRunLogUpdated OnRunLogUpdated;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Run")
    int32 MaxFloors = 12;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Run")
    int32 StartMaxHP = 72;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Run")
    int32 StartGold = 99;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Run")
    int32 ShopRemoveCardCost = 75;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Run")
    TArray<FName> StarterDeck;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Run")
    TArray<FName> CardRewardPool;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Run")
    TArray<FName> RelicPool;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Run")
    int32 RunSeed = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Run")
    bool bRunActive = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Run")
    int32 Gold = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Run")
    int32 CurrentHP = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Run")
    int32 MaxHP = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Run")
    FMapNodeData CurrentNode;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Run")
    TArray<FName> DeckCardIds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Run")
    TArray<FName> RelicIds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Run")
    TArray<FString> RunLog;

private:
    EMapNodeType ResolveNodeTypeByFloor(int32 Floor) const;
    FName PickRandomFromPool(const TArray<FName>& Pool) const;
    void AddRunLog(const FString& Line);
};
