#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RogueTypes.h"
#include "ShopManager.generated.h"

class ARunManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopRefreshed, const TArray<FShopCardOffer>&, CardOffers);

UCLASS()
class ANIMEROGUE_API AShopManager : public AActor
{
    GENERATED_BODY()

public:
    AShopManager();

    UFUNCTION(BlueprintCallable)
    void GenerateShop(ARunManager* RunManager);

    UFUNCTION(BlueprintCallable)
    bool BuyCardOffer(int32 OfferIndex, ARunManager* RunManager);

    UFUNCTION(BlueprintCallable)
    bool BuyRelic(ARunManager* RunManager);

    UFUNCTION(BlueprintCallable)
    bool RemoveCardService(int32 DeckIndex, ARunManager* RunManager);

    UFUNCTION(BlueprintPure)
    TArray<FShopCardOffer> GetCurrentOffers() const { return CurrentOffers; }

    UPROPERTY(BlueprintAssignable)
    FOnShopRefreshed OnShopRefreshed;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shop")
    int32 CardOfferCount = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shop")
    int32 CardPriceMin = 45;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shop")
    int32 CardPriceMax = 110;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Shop")
    int32 RelicPrice = 150;

private:
    UPROPERTY()
    TArray<FShopCardOffer> CurrentOffers;
};
