#include "StateMachine/HierarchyNode.h"
#include "StateMachine/IStateMachineLike.h"
#include "Utils/UtilsLibrary.h"

void UHierarchyNode::Initialize_Inner_Implementation()
{
	UStateNode::Initialize_Inner_Implementation();

	FString Address = this->Slot.MachineName.ToString();

	if (this->SubMachine)
	{
		switch (this->StateMachineSource)
		{
			case EHierarchyInputType::INLINED:
				this->SubMachine->SetParentMachine(this->GetMachine());
				break;
			case EHierarchyInputType::DEFINED: break;
		}
		this->SubMachine->Initialize(this->GetMachine()->GetActorOwner());
	}
	else if (auto Machine = Cast<UStateMachine>(this->GetMachine()->GetSubMachine(Address)))
	{
		this->SubMachine = Machine;
	}

	if (this->SubMachine)
	{
		this->SubMachine->OnTransitionFinished.AddDynamic(this, &UHierarchyNode::StateChangedCallback);
	}
}

void UHierarchyNode::PerformExit()
{
	if (this->SubMachine != nullptr) {
		FName SubStateName = this->SubMachine->GetCurrentStateName();

		if (this->ExitStates.Contains(SubStateName))
		{
			this->GetMachine()->Event(this->ExitStates[SubStateName].GetEvent());
		}
	}
}

void UHierarchyNode::Event_Inner_Implementation(FName EName){
	if (this->SubMachine)
	{
		this->SubMachine->SendEvent(EName);
	}
}

void UHierarchyNode::EventWithData_Inner_Implementation(FName EName, UObject* Data)
{
	if (this->SubMachine)
	{
		this->SubMachine->SendEventWithData(EName, Data);
	}
}

void UHierarchyNode::PostTransition_Inner_Implementation()
{
	this->PerformExit();
}

void UHierarchyNode::Enter_Inner_Implementation()
{
	if (this->SubMachine) {
		this->SubMachine->SetActive(true);
		if (this->ResetOnEnter)
		{
			this->SubMachine->Reset();
		}
		else
		{
			this->SubMachine->SendEvent(this->EnterEventName);
		}
	}
}

void UHierarchyNode::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->SubMachine)
	{
		this->SubMachine->Tick(DeltaTime);
		this->PerformExit();
	}
}

void UHierarchyNode::Exit_Inner_Implementation()
{
	if (this->SubMachine)
	{
		if (this->ResetOnExit)
		{
			this->SubMachine->Reset();
		}
		else
		{
			this->SubMachine->SendEvent(this->ExitEventName);
		}
		this->SubMachine->SetActive(false);
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
	if (this->SubMachine)
	{
		if (auto Node = this->SubMachine->GetCurrentState()->GetNode())
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
	if (this->StateMachineSource == EHierarchyInputType::DEFINED)
	{
		this->SubMachine = nullptr;
	}
	else if (this->StateMachineSource == EHierarchyInputType::INLINED)
	{
		this->Slot.MachineName = NAME_None;
	}
}

TArray<FString> UHierarchyNode::GetSubMachineStateOptions() const
{
	TArray<FString> Names;

	if (IsValid(this->SubMachine))
	{
		Names.Append(this->SubMachine->GetStateOptions(this));
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UHierarchyNode::GetSubMachineTransitionEvents() const
{
	TArray<FString> Names;

	if (IsValid(this->SubMachine))
	{
		for (const auto& Name : this->SubMachine->GetEvents())
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

	if (this->SubMachine)
	{
		Names = this->SubMachine->GetStateOptions(this);
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

#endif