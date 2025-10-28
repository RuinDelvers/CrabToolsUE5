#include "StateMachine/HierarchyNode.h"
#include "StateMachine/IStateMachineLike.h"
#include "Utils/UtilsLibrary.h"

void UHierarchyNode::Initialize_Inner_Implementation()
{
	UStateNode::Initialize_Inner_Implementation();

	this->Slot.Initialize(this->GetMachine());
	/*
	if (this->SubMachine)
	{
		switch (this->StateMachineSource)
		{
			case EHierarchyInputType::INLINED:
				this->SubMachine->SetParentMachine(this->GetMachine());
				break;
			case EHierarchyInputType::DEFINED: break;
		}
		this->SubMachine->Initialize(this->GetMachine()->GetOwner());
	}
	else if (auto Machine = this->GetMachine()->GetSubMachine(this->Slot))
	{
		this->SubMachine = Machine;
	}
	*/

	if (this->Slot)
	{
		this->Slot->OnTransitionFinished.AddDynamic(this, &UHierarchyNode::StateChangedCallback);
	}
}

void UHierarchyNode::PerformExit()
{
	if (this->Slot)
	{
		FName SubStateName = this->Slot->GetCurrentStateName();

		if (this->ExitStates.Contains(SubStateName))
		{
			this->GetMachine()->Event(this->ExitStates[SubStateName].GetEvent());
		}
	}
}

void UHierarchyNode::Event_Inner_Implementation(FName InEvent)
{
	if (this->Slot)
	{
		this->Slot->SendEvent(InEvent);
	}
}

void UHierarchyNode::EventWithData_Inner_Implementation(FName InEvent, UObject* Data)
{
	if (this->Slot)
	{
		this->Slot->SendEventWithData(InEvent, Data);
	}
}

bool UHierarchyNode::DoesReferenceMachine_Inner_Implementation(FName MachineName) const
{
	return this->Slot.DoesReferenceMachine(MachineName);
}

void UHierarchyNode::Enter_Inner_Implementation()
{
	if (this->Slot)
	{
		this->Slot->SetActive(true);

		if (this->ResetOnEnter)
		{
			this->Slot->Reset();
		}
		else
		{
			this->Slot->SendEvent(this->EnterEventName);
		}

		if (this->bPropagateEnterEvent)
		{
			this->Slot->SendEvent(this->GetMachine()->GetCurrentEvent());
		}

		this->PerformExit();
	}
}

void UHierarchyNode::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->Slot)
	{
		this->Slot->Tick(DeltaTime);
		this->PerformExit();
	}
}

void UHierarchyNode::Exit_Inner_Implementation()
{
	if (this->Slot)
	{
		if (this->ResetOnExit)
		{
			this->Slot->Reset();
		}
		else
		{
			this->Slot->SendEvent(this->ExitEventName);
		}
		this->Slot->SetActive(false);
	}
}

FName FHierarchyEventValue::GetEvent() const
{
	if (this->EventType == EHierarchyInputType::INLINED)
	{
		return this->InlinedEvent;
	}
	else
	{
		return this->DefinedEvent;
	}
}

void UHierarchyNode::StateChangedCallback(UStateMachine* Data)
{
	if (this->Active())
	{
		FName StateName = Data->GetCurrentStateName();

		if (this->ExitStates.Contains(StateName))
		{
			this->EmitEvent(this->ExitStates[StateName].GetEvent());
		}

		if (this->Active())
		{
			this->UpdateTickRequirements();
		}
	}
}

bool UHierarchyNode::RequiresTick_Implementation() const
{
	if (this->Slot)
	{
		if (auto Node = this->Slot->GetCurrentState()->GetNode())
		{
			return Node->RequiresTick();
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

// Editor helper functions.
#if WITH_EDITOR
void UHierarchyNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Super::GetEmittedEvents(Events);

	if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		Events.Append(Outer->GetEmittedEvents());
	}

	for (const auto& Values : this->ExitStates)
	{
		Events.Add(Values.Value.GetEvent());
	}
}

void UHierarchyNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	this->Slot.Validate();
}

TArray<FString> UHierarchyNode::GetSubMachineStateOptions() const
{
	TArray<FString> Names;

	if (IsValid(this->Slot))
	{
		Names.Append(this->Slot->GetStateOptions(this));
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UHierarchyNode::GetSubMachineTransitionEvents() const
{
	TArray<FString> Names;

	if (IsValid(this->Slot))
	{
		for (const auto& Name : this->Slot->GetEvents())
		{
			Names.Add(Name.ToString());
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}


TArray<FString> UHierarchyNode::GetStateEventOptions() const
{
	TArray<FString> Names;	

	if (this->Slot)
	{
		Names = this->Slot->GetStateOptions(this);
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

#endif