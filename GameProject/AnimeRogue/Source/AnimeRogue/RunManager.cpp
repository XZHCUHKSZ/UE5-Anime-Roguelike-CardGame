#include "RunManager.h"

ARunManager::ARunManager()
{
    PrimaryActorTick.bCanEverTick = false;

    StarterDeck = {
        "card_strike_01",
        "card_strike_01",
        "card_strike_01",
        "card_strike_01",
        "card_strike_01",
        "card_guard_01",
        "card_guard_01",
        "card_guard_01",
        "card_guard_01",
        "card_focus_01"
    };

    CardRewardPool = {
        "card_strike_01",
        "card_guard_01",
        "card_focus_01",
        "swg_quick_01",
        "swg_bleed_01",
        "swg_finisher_01",
        "swg_setup_01"
    };

    RelicPool = {
        "relic_blood_charm",
        "relic_coin_eye",
        "relic_arcane_leaf"
    };
}

void ARunManager::StartRun(const int32 Seed)
{
    RunSeed = Seed > 0 ? Seed : FMath::Rand();
    bRunActive = true;

    MaxHP = FMath::Max(1, StartMaxHP);
    CurrentHP = MaxHP;
    Gold = FMath::Max(0, StartGold);

    DeckCardIds = StarterDeck;
    RelicIds.Reset();
    bHasPendingEvent = false;
    PendingEvent = {};
    bHasPendingShop = false;
    PendingShopOffers.Reset();
    RunLog.Reset();

    CurrentNode = {};
    CurrentNode.Floor = 1;
    CurrentNode.NodeIndex = 0;
    CurrentNode.NodeType = ResolveNodeTypeByFloor(CurrentNode.Floor);
    OnNodeAdvanced.Broadcast(CurrentNode);

    AddRunLog(FString::Printf(TEXT("Run started. Seed=%d, HP=%d, Gold=%d"), RunSeed, CurrentHP, Gold));
}

bool ARunManager::AdvanceToNextNode()
{
    if (!bRunActive || CurrentNode.Floor >= MaxFloors)
    {
        return false;
    }

    CurrentNode.Floor += 1;
    CurrentNode.NodeIndex += 1;
    CurrentNode.NodeType = ResolveNodeTypeByFloor(CurrentNode.Floor);
    OnNodeAdvanced.Broadcast(CurrentNode);
    AddRunLog(FString::Printf(TEXT("Advance to floor %d, node=%d, type=%d"), CurrentNode.Floor, CurrentNode.NodeIndex, static_cast<int32>(CurrentNode.NodeType)));
    return true;
}

void ARunManager::GainGold(const int32 Amount)
{
    if (Amount <= 0)
    {
        return;
    }

    Gold += Amount;
    AddRunLog(FString::Printf(TEXT("Gain %d gold. Current gold=%d"), Amount, Gold));
}

bool ARunManager::SpendGold(const int32 Amount)
{
    if (Amount <= 0)
    {
        return true;
    }

    if (Gold < Amount)
    {
        AddRunLog(FString::Printf(TEXT("Spend gold failed. Need=%d, Current=%d"), Amount, Gold));
        return false;
    }

    Gold -= Amount;
    AddRunLog(FString::Printf(TEXT("Spend %d gold. Current gold=%d"), Amount, Gold));
    return true;
}

void ARunManager::HealPlayer(const int32 Amount)
{
    if (Amount <= 0)
    {
        return;
    }

    CurrentHP = FMath::Min(MaxHP, CurrentHP + Amount);
    AddRunLog(FString::Printf(TEXT("Heal %d HP. Current HP=%d/%d"), Amount, CurrentHP, MaxHP));
}

void ARunManager::DamagePlayer(const int32 Amount)
{
    if (Amount <= 0)
    {
        return;
    }

    CurrentHP = FMath::Max(0, CurrentHP - Amount);
    AddRunLog(FString::Printf(TEXT("Take %d damage. Current HP=%d/%d"), Amount, CurrentHP, MaxHP));
    if (CurrentHP <= 0)
    {
        bRunActive = false;
        AddRunLog(TEXT("Run ended: player defeated."));
    }
}

void ARunManager::AddCardToDeck(const FName CardId)
{
    if (CardId.IsNone())
    {
        return;
    }

    DeckCardIds.Add(CardId);
    AddRunLog(FString::Printf(TEXT("Add card to deck: %s"), *CardId.ToString()));
}

bool ARunManager::RemoveCardAt(const int32 DeckIndex)
{
    if (!DeckCardIds.IsValidIndex(DeckIndex))
    {
        return false;
    }

    const FName Removed = DeckCardIds[DeckIndex];
    DeckCardIds.RemoveAt(DeckIndex);
    AddRunLog(FString::Printf(TEXT("Remove card from deck: %s"), *Removed.ToString()));
    return true;
}

bool ARunManager::BuyCard(const FName CardId, const int32 Price)
{
    if (CardId.IsNone())
    {
        return false;
    }

    if (!SpendGold(Price))
    {
        return false;
    }

    AddCardToDeck(CardId);
    return true;
}

bool ARunManager::BuyRandomRelic(const int32 Price)
{
    if (!SpendGold(Price))
    {
        return false;
    }

    const FName RelicId = PickRandomFromPool(RelicPool);
    if (RelicId.IsNone())
    {
        return false;
    }

    RelicIds.Add(RelicId);
    AddRunLog(FString::Printf(TEXT("Buy relic: %s"), *RelicId.ToString()));
    return true;
}

bool ARunManager::RemoveCardAtShop(const int32 DeckIndex)
{
    if (!SpendGold(ShopRemoveCardCost))
    {
        return false;
    }

    return RemoveCardAt(DeckIndex);
}

bool ARunManager::ExecuteEventScript(const FName EventScriptId)
{
    if (EventScriptId.IsNone())
    {
        return false;
    }

    const FString Id = EventScriptId.ToString();

    if (Id == "event_gain_gold_50")
    {
        GainGold(50);
        return true;
    }

    if (Id == "event_gain_gold_40")
    {
        GainGold(40);
        return true;
    }

    if (Id == "event_heal_small")
    {
        HealPlayer(8);
        return true;
    }

    if (Id == "event_heal_20pct")
    {
        HealPlayer(FMath::Max(1, MaxHP / 5));
        return true;
    }

    if (Id == "event_take_damage_get_relic")
    {
        DamagePlayer(10);
        const FName RelicId = PickRandomFromPool(RelicPool);
        if (!RelicId.IsNone())
        {
            RelicIds.Add(RelicId);
            AddRunLog(FString::Printf(TEXT("Event relic gained: %s"), *RelicId.ToString()));
        }
        return true;
    }

    if (Id == "event_remove_card")
    {
        if (!DeckCardIds.IsEmpty())
        {
            RemoveCardAt(FMath::RandRange(0, DeckCardIds.Num() - 1));
        }
        return true;
    }

    if (Id == "event_add_skill_card" || Id == "event_gain_attack_card" || Id == "event_gain_defense_card")
    {
        const FName CardId = PickRandomFromPool(CardRewardPool);
        AddCardToDeck(CardId);
        return true;
    }

    if (Id == "event_buy_random_relic")
    {
        const FName RelicId = PickRandomFromPool(RelicPool);
        if (!RelicId.IsNone())
        {
            RelicIds.Add(RelicId);
            AddRunLog(FString::Printf(TEXT("Event relic gained: %s"), *RelicId.ToString()));
        }
        return true;
    }

    if (Id == "event_pay_hp_get_rare_card" || Id == "event_curse_for_rare")
    {
        DamagePlayer(10);
        AddCardToDeck("swg_signature_01");
        return true;
    }

    if (Id == "event_nothing" || Id == "event_leave")
    {
        AddRunLog(TEXT("Event result: nothing happened."));
        return true;
    }

    AddRunLog(FString::Printf(TEXT("Event script not implemented: %s"), *Id));
    return false;
}

void ARunManager::FinishRun(const bool bWon)
{
    bRunActive = false;
    bHasPendingEvent = false;
    PendingEvent = {};
    bHasPendingShop = false;
    PendingShopOffers.Reset();
    AddRunLog(bWon ? TEXT("Run complete: victory.") : TEXT("Run complete: defeated."));
}

void ARunManager::SetPendingEvent(const FResolvedEventData& EventData)
{
    PendingEvent = EventData;
    bHasPendingEvent = true;
}

void ARunManager::ClearPendingEvent()
{
    PendingEvent = {};
    bHasPendingEvent = false;
}

void ARunManager::SetPendingShopOffers(const TArray<FShopCardOffer>& InOffers)
{
    PendingShopOffers = InOffers;
    bHasPendingShop = !PendingShopOffers.IsEmpty();
}

void ARunManager::ClearPendingShopOffers()
{
    PendingShopOffers.Reset();
    bHasPendingShop = false;
}

FRunSaveData ARunManager::BuildRunSaveData() const
{
    FRunSaveData Data;
    Data.bRunActive = bRunActive;
    Data.RunSeed = RunSeed;
    Data.Gold = Gold;
    Data.CurrentHP = CurrentHP;
    Data.MaxHP = MaxHP;
    Data.CurrentNode = CurrentNode;
    Data.DeckCardIds = DeckCardIds;
    Data.RelicIds = RelicIds;
    Data.bHasPendingEvent = bHasPendingEvent;
    Data.PendingEvent = PendingEvent;
    Data.bHasPendingShop = bHasPendingShop;
    Data.PendingShopOffers = PendingShopOffers;
    return Data;
}

void ARunManager::ApplyRunSaveData(const FRunSaveData& InData)
{
    bRunActive = InData.bRunActive;
    RunSeed = InData.RunSeed;
    Gold = InData.Gold;
    CurrentHP = FMath::Max(0, InData.CurrentHP);
    MaxHP = FMath::Max(1, InData.MaxHP);
    CurrentNode = InData.CurrentNode;
    DeckCardIds = InData.DeckCardIds;
    RelicIds = InData.RelicIds;
    bHasPendingEvent = InData.bHasPendingEvent;
    PendingEvent = InData.PendingEvent;
    bHasPendingShop = InData.bHasPendingShop;
    PendingShopOffers = InData.PendingShopOffers;

    OnNodeAdvanced.Broadcast(CurrentNode);
    AddRunLog(FString::Printf(TEXT("Run loaded. Seed=%d, HP=%d/%d, Gold=%d"), RunSeed, CurrentHP, MaxHP, Gold));
}

EMapNodeType ARunManager::ResolveNodeTypeByFloor(const int32 Floor) const
{
    if (Floor >= MaxFloors)
    {
        return EMapNodeType::Boss;
    }

    if (Floor == 4 || Floor == 8)
    {
        return EMapNodeType::Elite;
    }

    if (Floor == 3 || Floor == 7 || Floor == 10)
    {
        return EMapNodeType::Event;
    }

    if (Floor == 5 || Floor == 9)
    {
        return EMapNodeType::Shop;
    }

    if (Floor == 6 || Floor == 11)
    {
        return EMapNodeType::Rest;
    }

    return EMapNodeType::Battle;
}

FName ARunManager::PickRandomFromPool(const TArray<FName>& Pool) const
{
    if (Pool.IsEmpty())
    {
        return NAME_None;
    }

    const int32 Index = FMath::RandRange(0, Pool.Num() - 1);
    return Pool[Index];
}

void ARunManager::AddRunLog(const FString& Line)
{
    RunLog.Add(Line);
    if (RunLog.Num() > 60)
    {
        RunLog.RemoveAt(0, RunLog.Num() - 60);
    }

    OnRunLogUpdated.Broadcast(Line);
}
