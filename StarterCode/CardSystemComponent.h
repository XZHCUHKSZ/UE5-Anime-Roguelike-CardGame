#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RogueTypes.h"
#include "CardSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeckChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class YOURMODULE_API UCardSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnDeckChanged OnDeckChanged;

    UFUNCTION(BlueprintCallable)
    void InitializeDeck(const TArray<FName>& InCardIds);

    UFUNCTION(BlueprintCallable)
    void DrawCards(int32 Count);

    UFUNCTION(BlueprintCallable)
    bool TryPlayCard(int32 HandIndex, int32 CurrentEnergy);

    UFUNCTION(BlueprintCallable)
    void DiscardHand();

    UFUNCTION(BlueprintPure)
    const TArray<FRuntimeCard>& GetHand() const { return Hand; }

private:
    UPROPERTY()
    TArray<FRuntimeCard> DrawPile;

    UPROPERTY()
    TArray<FRuntimeCard> Hand;

    UPROPERTY()
    TArray<FRuntimeCard> DiscardPile;

    void ShuffleIntoDrawPile();
};
