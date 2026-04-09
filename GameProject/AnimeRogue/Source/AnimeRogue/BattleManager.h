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
    TArray<FCardRewardOption> GetCurrentRewardOptions() const { return CurrentRewardOptions; }

    UFUNCTION(BlueprintCallable)
    bool PickRewardCard(int32 OptionIndex, bool bSkip);

    UFUNCTION(BlueprintPure)
    int32 GetCurrentEnergy() const { return CurrentEnergy; }

    UFUNCTION(BlueprintPure)
    EBattlePhase GetCurrentPhase() const { return CurrentPhase; }

    UFUNCTION(BlueprintPure)
    UCardSystemComponent* GetCardSystem() const { return CardSystem; }

    UFUNCTION(BlueprintPure)
    FUnitState GetPlayerState() const { return PlayerState; }

    UFUNCTION(BlueprintPure)
    FUnitState GetEnemyState() const { return EnemyState; }

    UFUNCTION(BlueprintPure)
    FString GetCurrentEnemyIntent() const { return CurrentEnemyIntent; }

    UFUNCTION(BlueprintPure)
    TArray<FString> GetBattleLog() const { return BattleLog; }

    UFUNCTION(BlueprintPure)
    FBattleSaveData BuildBattleSaveData() const;

    UFUNCTION(BlueprintCallable)
    void ApplyBattleSaveData(const FBattleSaveData& InData);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
    int32 EnergyPerTurn = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
    int32 DrawPerTurn = 5;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
    TArray<FName> StarterDeck;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
    UDataTable* CardDataTable = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
    UDataTable* EnemyIntentScriptTable = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
    FName EnemyIntentScriptId = "intent_basic_melee";

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
    int32 PlayerStartHP = 72;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
    int32 EnemyStartHP = 40;

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

    UPROPERTY()
    FUnitState PlayerState;

    UPROPERTY()
    FUnitState EnemyState;

    UPROPERTY()
    FString CurrentEnemyIntent;

    UPROPERTY()
    TArray<FString> EnemyIntentSequence;

    UPROPERTY()
    int32 EnemyIntentIndex = 0;

    UPROPERTY()
    TArray<FString> BattleLog;

    void EnterPlayerTurnStart();
    void EnterEnemyTurn();
    void GenerateCardRewards(int32 RewardCount);
    void InitializeBattleStates();
    void LoadEnemyIntentSequence();
    FString ResolveCurrentEnemyIntent() const;
    void ExecuteEnemyIntent(const FString& IntentToken);
    void AddBattleLog(const FString& Line);
    void ApplyDamageToPlayer(int32 Damage);
    void ApplyDamageToEnemy(int32 Damage);
    void GainPlayerBlock(int32 BlockValue);
    void GainEnemyBlock(int32 BlockValue);
    void ApplyCardEffectById(const FName& CardId);
    const FCardData* FindCardData(const FName& CardId) const;
    int32 GetStatusStack(const FUnitState& Unit, FName StatusId) const;
    void AddStatusStack(FUnitState& Unit, FName StatusId, int32 Delta);
    void DecayStatus(FUnitState& Unit, FName StatusId, int32 DecayValue = 1);
    int32 ModifyOutgoingDamage(int32 BaseDamage, const FUnitState& Attacker, bool bConsumeNextAttackBonus);
    int32 ModifyIncomingDamage(int32 Damage, const FUnitState& Defender) const;
    void ProcessTurnStartStatuses(FUnitState& Unit, bool bIsPlayer);
    void HealPlayer(int32 Value);
};
