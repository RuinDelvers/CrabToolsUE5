#include "StateMachine/General/EventComboNode.h"

void UEventComboNode::Event_Inner_Implementation(FName Event)
{
	this->ReceiveEvent(Event);

	if (this->Active())
	{
		Super::Event_Inner_Implementation(Event);
	}	
}

void UEventComboNode::EventWithData_Inner_Implementation(FName EName, UObject* Data)
{
	this->ReceiveEvent(EName);

	if (this->Active())
	{
		Super::EventWithData_Inner_Implementation(EName, Data);
	}
}

void UEventComboNode::ReceiveEvent(FName EName)
{
	for (auto& Data : this->Combos)
	{
		if (Data.Events.Contains(EName))
		{
			Data.Events[EName].Activate();

			if (Data.IsActive())
			{
				Data.Reset();
				this->EmitEvent(Data.EmittedEvent);
			}
		}
	}
}

bool FEventComboNodeData::IsActive() const
{
	for (const auto& Data : this->Events)
	{
		if (!Data.Value.bActive)
		{
			return false;
		}
	}

	return true;
}

void FEventComboNodeData::Reset()
{
	for (auto& Point : this->Events)
	{
		Point.Value.bActive = false;
	}
}

void FEventComboNodeDataPoint::Activate()
{
	this->bActive = true;

	switch (this->Type)
	{
		case EEventComboType::FRAME:
			if (GEngine)
			{
				this->TimeUp = GEngine->GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
					{
						this->TimeUpCallback();
					});
			}
			break;

		case EEventComboType::TIME:
			if (GEngine)
			{
				GEngine->GetWorld()->GetTimerManager().SetTimer(this->TimeUp, [this]()
					{
						this->TimeUpCallback();
					},
					this->Time,
					false);
			}
			break;
	}
}

void FEventComboNodeDataPoint::TimeUpCallback()
{
	this->bActive = false;
}

#if WITH_EDITORONLY_DATA
void UEventComboNode::GetEmittedEvents(TSet<FName>& Events) const
{
	for (const auto& Data : this->Combos)
	{
		Events.Add(Data.EmittedEvent);
	}
}

TArray<FString> UEventComboNode::GetEventComboOptions() const
{
	TSet<FName> ChildrenEvents;
	Super::GetEmittedEvents(ChildrenEvents);

	TArray<FString> Names;

	Names.Sort([](const FString& A, const FString& B) { return A < B; });

	return Names;
}
#endif