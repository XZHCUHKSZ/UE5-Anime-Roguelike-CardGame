#include "CardSystemComponent.h"

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

bool UCardSystemComponent::TryPlayCard(const int32 HandIndex, const int32 CurrentEnergy)
{
    if (!Hand.IsValidIndex(HandIndex))
    {
        return false;
    }

    const FRuntimeCard Card = Hand[HandIndex];
    const int32 CardCost = FMath::Max(0, 1 + Card.TempCostOffset);
    if (CurrentEnergy < CardCost)
    {
        return false;
    }

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
