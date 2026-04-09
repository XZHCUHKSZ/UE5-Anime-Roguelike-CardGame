#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueTypes.h"
#include "BattleManager.generated.h"

class UCardSystemComponent;

UCLASS()
class YOURMODULE_API ABattleManager : public AActor
{
    GENERATED_BODY()

public:
    ABattleManager();

    UFUNCTION(BlueprintCallable)
    void StartBattle();

    UFUNCTION(BlueprintCallable)
    void EndPlayerTurn();

    UFUNCTION(BlueprintCallable)
    void ConsumePlayerEnergy(int32 Cost);

    UFUNCTION(BlueprintPure)
    int32 GetCurrentEnergy() const { return CurrentEnergy; }

    UFUNCTION(BlueprintPure)
    EBattlePhase GetCurrentPhase() const { return CurrentPhase; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
    int32 EnergyPerTurn = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
    int32 DrawPerTurn = 5;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle")
    EBattlePhase CurrentPhase = EBattlePhase::BattleStart;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle")
    int32 CurrentEnergy = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle")
    UCardSystemComponent* CardSystem = nullptr;

private:
    void EnterPlayerTurnStart();
    void EnterEnemyTurn();
};
