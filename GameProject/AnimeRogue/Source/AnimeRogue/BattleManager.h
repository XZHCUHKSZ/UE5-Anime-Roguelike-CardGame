#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueTypes.h"
#include "BattleManager.generated.h"

class UCardSystemComponent;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleRewardReady, TArray<FCardRewardOption>, RewardOptions);

UCLASS()
class ANIMEROGUE_API ABattleManager : public AActor
{
    GENERATED_BODY()

public:
    ABattleManager();

    UFUNCTION(BlueprintCallable)
    void StartBattle();

    UFUNCTION(BlueprintCallable)
    bool TryPlayCardFromHand(int32 HandIndex);

    UFUNCTION(BlueprintCallable)
    void EndPlayerTurn();

    UFUNCTION(BlueprintCallable)
    void CompleteBattle(bool bPlayerWon);

    UFUNCTION(BlueprintPure)
    const TArray<FCardRewardOption>& GetCurrentRewardOptions() const { return CurrentRewardOptions; }

    UFUNCTION(BlueprintCallable)
    bool PickRewardCard(int32 OptionIndex, bool bSkip);

    UFUNCTION(BlueprintPure)
    int32 GetCurrentEnergy() const { return CurrentEnergy; }

    UFUNCTION(BlueprintPure)
    EBattlePhase GetCurrentPhase() const { return CurrentPhase; }

    UFUNCTION(BlueprintPure)
    UCardSystemComponent* GetCardSystem() const { return CardSystem; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
    int32 EnergyPerTurn = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
    int32 DrawPerTurn = 5;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
    TArray<FName> StarterDeck;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
    UDataTable* CardDataTable = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reward")
    TArray<FName> RewardCardPool;

    UPROPERTY(BlueprintAssignable, Category="Battle")
    FOnBattleRewardReady OnBattleRewardReady;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle")
    EBattlePhase CurrentPhase = EBattlePhase::BattleStart;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle")
    int32 CurrentEnergy = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle")
    UCardSystemComponent* CardSystem = nullptr;

private:
    UPROPERTY()
    TArray<FCardRewardOption> CurrentRewardOptions;

    void EnterPlayerTurnStart();
    void EnterEnemyTurn();
    void GenerateCardRewards(int32 RewardCount);
};
