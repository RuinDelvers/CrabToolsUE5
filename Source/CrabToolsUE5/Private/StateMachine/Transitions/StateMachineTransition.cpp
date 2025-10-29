#include "StateMachine/Transitions/StateMachineTransition.h"

void UStateMachineTransitionCondition::Initialize_Inner_Implementation()
{
	this->Slot.Initialize(this->GetMachine());
}

void UStateMachineTransitionCondition::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->Slot)
	{
		this->Slot->Tick(DeltaTime);
	}
}

bool UStateMachineTransitionCondition::Check() const
{
	if (this->Slot)
	{
		return this->ValidStates.Contains(this->Slot->GetCurrentStateName());
	}
	else
	{
		return false;
	}
}

void UStateMachineTransitionCondition::Exit_Inner_Implementation()
{
	if (this->Slot)
	{
		if (this->bResetOnExit)
		{
			this->Slot->Reset();
		}

		this->Slot->SetActive(false);
	}
}

void UStateMachineTransitionCondition::Event_Inner_Implementation(FName InEvent)
{
	if (this->Slot)
	{
		this->Slot->SendEvent(InEvent);
	}
}

void UStateMachineTransitionCondition::EventWithData_Inner_Implementation(FName InEvent, UObject* Data)
{
	if (this->Slot)
	{
		this->Slot->SendEventWithData(InEvent, Data);
	}
}

#pragma region Data Condition

void UStateMachineTransitionDataCondition::Initialize_Inner_Implementation()
{
	this->Slot.Initialize(this->GetMachine());
}

void UStateMachineTransitionDataCondition::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->Slot)
	{
		this->Slot->Tick(DeltaTime);
	}
}

bool UStateMachineTransitionDataCondition::Check(UObject* Data) const
{
	if (this->Slot)
	{
		return this->ValidStates.Contains(this->Slot->GetCurrentStateName());
	}
	else
	{
		return false;
	}
}

void UStateMachineTransitionDataCondition::Exit_Inner_Implementation()
{
	if (this->Slot)
	{
		if (this->bResetOnExit)
		{
			this->Slot->Reset();
		}

		this->Slot->SetActive(false);
	}
}

void UStateMachineTransitionDataCondition::Event_Inner_Implementation(FName InEvent)
{
	if (this->Slot)
	{
		this->Slot->SendEvent(InEvent);
	}
}

void UStateMachineTransitionDataCondition::EventWithData_Inner_Implementation(FName InEvent, UObject* Data)
{
	if (this->Slot)
	{
		this->Slot->SendEventWithData(InEvent, Data);
	}
}

#pragma endregion