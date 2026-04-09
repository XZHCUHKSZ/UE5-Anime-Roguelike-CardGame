#include "EventManager.h"
#include "Engine/DataTable.h"
#include "RunManager.h"

AEventManager::AEventManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

bool AEventManager::RollRandomEvent()
{
    if (!EventDataTable)
    {
        return false;
    }

    TArray<FName> Pool = EventPool;
    if (Pool.IsEmpty())
    {
        Pool = EventDataTable->GetRowNames();
    }

    if (Pool.IsEmpty())
    {
        return false;
    }

    const int32 Index = FMath::RandRange(0, Pool.Num() - 1);
    return ResolveEventFromRowName(Pool[Index]);
}

bool AEventManager::ChooseOption(const int32 OptionIndex, ARunManager* RunManager)
{
    if (!RunManager || !CurrentEvent.Options.IsValidIndex(OptionIndex))
    {
        return false;
    }

    const FName ScriptId = CurrentEvent.Options[OptionIndex].ScriptId;
    const bool bOk = RunManager->ExecuteEventScript(ScriptId);
    CurrentEvent = {};
    return bOk;
}

void AEventManager::SetCurrentEvent(const FResolvedEventData& InEvent)
{
    CurrentEvent = InEvent;
    OnEventRolled.Broadcast(CurrentEvent);
}

void AEventManager::ClearCurrentEvent()
{
    CurrentEvent = {};
}

bool AEventManager::ResolveEventFromRowName(const FName& RowName)
{
    if (!EventDataTable)
    {
        return false;
    }

    const FEventDataRow* Row = EventDataTable->FindRow<FEventDataRow>(RowName, TEXT("ResolveEventFromRowName"));
    if (!Row)
    {
        return false;
    }

    FResolvedEventData EventData;
    EventData.EventId = Row->EventId;
    EventData.Title = Row->Title;

    FEventOptionData OptionA;
    OptionA.Text = Row->OptionA;
    OptionA.ScriptId = Row->ResultAScript;
    EventData.Options.Add(OptionA);

    FEventOptionData OptionB;
    OptionB.Text = Row->OptionB;
    OptionB.ScriptId = Row->ResultBScript;
    EventData.Options.Add(OptionB);

    FEventOptionData OptionC;
    OptionC.Text = Row->OptionC;
    OptionC.ScriptId = Row->ResultCScript;
    EventData.Options.Add(OptionC);

    CurrentEvent = EventData;
    OnEventRolled.Broadcast(CurrentEvent);
    return true;
}
