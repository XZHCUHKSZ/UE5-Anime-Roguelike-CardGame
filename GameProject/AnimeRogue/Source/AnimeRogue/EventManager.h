#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueTypes.h"
#include "EventManager.generated.h"

class UDataTable;
class ARunManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventRolled, const FResolvedEventData&, EventData);

UCLASS()
class ANIMEROGUE_API AEventManager : public AActor
{
    GENERATED_BODY()

public:
    AEventManager();

    UFUNCTION(BlueprintCallable)
    bool RollRandomEvent();

    UFUNCTION(BlueprintCallable)
    bool ChooseOption(int32 OptionIndex, ARunManager* RunManager);

    UFUNCTION(BlueprintPure)
    FResolvedEventData GetCurrentEvent() const { return CurrentEvent; }

    UFUNCTION(BlueprintCallable)
    void SetCurrentEvent(const FResolvedEventData& InEvent);

    UFUNCTION(BlueprintCallable)
    void ClearCurrentEvent();

    UPROPERTY(BlueprintAssignable)
    FOnEventRolled OnEventRolled;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
    UDataTable* EventDataTable = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
    TArray<FName> EventPool;

private:
    UPROPERTY()
    FResolvedEventData CurrentEvent;

    bool ResolveEventFromRowName(const FName& RowName);
};
