#include "CardSystemComponent.h"

void UCardSystemComponent::SetCardDataTable(UDataTable* InCardDataTable)
{
    CardDataTable = InCardDataTable;
}

void UCardSystemComponent::InitializeDeck(const TArray<FName>& InCardIds)
{
    DrawPile.Reset();
    Hand.Reset();
    DiscardPile.Reset();

    for (const FName CardId : InCardIds)
    {
        FRuntimeCard NewCard;
        NewCard.CardId = CardId;
        DrawPile.Add(NewCard);
    }

    ShuffleIntoDrawPile();
    OnDeckChanged.Broadcast();
}

void UCardSystemComponent::DrawCards(const int32 Count)
{
    for (int32 i = 0; i < Count; ++i)
    {
        if (DrawPile.IsEmpty())
        {
            if (DiscardPile.IsEmpty())
            {
                break;
            }

            DrawPile = DiscardPile;
            DiscardPile.Reset();
            ShuffleIntoDrawPile();
        }

        Hand.Add(DrawPile.Pop(false));
    }

    OnDeckChanged.Broadcast();
}

bool UCardSystemComponent::TryPlayCard(const int32 HandIndex, const int32 CurrentEnergy, int32& OutCost)
{
    OutCost = 0;
    if (!Hand.IsValidIndex(HandIndex))
    {
        return false;
    }

    const FRuntimeCard Card = Hand[HandIndex];
    const int32 CardCost = ResolveCardCost(Card);
    if (CurrentEnergy < CardCost)
    {
        return false;
    }

    OutCost = CardCost;
    DiscardPile.Add(Card);
    Hand.RemoveAt(HandIndex);
    OnDeckChanged.Broadcast();
    return true;
}

void UCardSystemComponent::DiscardHand()
{
    DiscardPile.Append(Hand);
    Hand.Reset();
    OnDeckChanged.Broadcast();
}

void UCardSystemComponent::ShuffleIntoDrawPile()
{
    for (int32 i = DrawPile.Num() - 1; i > 0; --i)
    {
        const int32 SwapIndex = FMath::RandRange(0, i);
        DrawPile.Swap(i, SwapIndex);
    }
}

int32 UCardSystemComponent::ResolveCardCost(const FRuntimeCard& Card) const
{
    int32 BaseCost = 1;
    if (CardDataTable)
    {
        const FCardData* Row = CardDataTable->FindRow<FCardData>(Card.CardId, TEXT("ResolveCardCost"));
        if (Row)
        {
            BaseCost = Row->Cost;
        }
    }

    return FMath::Max(0, BaseCost + Card.TempCostOffset);
}
