#include "ShopManager.h"
#include "RunManager.h"

AShopManager::AShopManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AShopManager::GenerateShop(ARunManager* RunManager)
{
    CurrentOffers.Reset();
    if (!RunManager)
    {
        OnShopRefreshed.Broadcast(CurrentOffers);
        return;
    }

    const TArray<FName> Deck = RunManager->GetDeck();
    TArray<FName> Pool = Deck;
    if (Pool.IsEmpty())
    {
        Pool = {
            "card_strike_01",
            "card_guard_01",
            "card_focus_01",
            "swg_quick_01",
            "swg_bleed_01"
        };
    }

    for (int32 i = 0; i < CardOfferCount; ++i)
    {
        const int32 Idx = FMath::RandRange(0, Pool.Num() - 1);

        FShopCardOffer Offer;
        Offer.CardId = Pool[Idx];
        Offer.Price = FMath::RandRange(CardPriceMin, CardPriceMax);
        CurrentOffers.Add(Offer);
    }

    OnShopRefreshed.Broadcast(CurrentOffers);
}

bool AShopManager::BuyCardOffer(const int32 OfferIndex, ARunManager* RunManager)
{
    if (!RunManager || !CurrentOffers.IsValidIndex(OfferIndex))
    {
        return false;
    }

    const FShopCardOffer Offer = CurrentOffers[OfferIndex];
    if (!RunManager->BuyCard(Offer.CardId, Offer.Price))
    {
        return false;
    }

    CurrentOffers.RemoveAt(OfferIndex);
    OnShopRefreshed.Broadcast(CurrentOffers);
    return true;
}

bool AShopManager::BuyRelic(ARunManager* RunManager)
{
    if (!RunManager)
    {
        return false;
    }

    return RunManager->BuyRandomRelic(RelicPrice);
}

bool AShopManager::RemoveCardService(const int32 DeckIndex, ARunManager* RunManager)
{
    if (!RunManager)
    {
        return false;
    }

    return RunManager->RemoveCardAtShop(DeckIndex);
}
