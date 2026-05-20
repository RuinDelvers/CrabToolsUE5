// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StateMachineComponent.h"
#include "Engine.h"

// Sets default values for this component's properties
UStateMachineComponent::UStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	if (this->Machine)
	{
		this->Machine->Initialize(this->GetOwner());		
	}
}

void UStateMachineComponent::CreateMachine(TSubclassOf<UStateMachine> MachineClass)
{
	auto NewMachine = NewObject<UStateMachine>(this, MachineClass);

	if (NewMachine)
	{
		this->Machine = NewMachine;
		this->Machine->Initialize(this->GetOwner());
	}
}

void UStateMachineComponent::Event_Direct(FName InEvent, UObject* Source)
{
	if (this->HasMachine())
	{
		this->Machine->SendEvent(InEvent, Source);
	}
}

void UStateMachineComponent::EventWithData_Direct(FName InEvent, UObject* Data, UObject* Source)
{
	if (this->HasMachine()) {
		this->Machine->SendEventWithData(InEvent, Data, Source);
	}
}

FName UStateMachineComponent::CurrentStateName()
{
	return this->Machine->GetCurrentStateName();
}


bool UStateMachineComponent::HasMachine()
{
	return this->Machine != nullptr;
}

#if WITH_EDITOR
void UStateMachineComponent::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);
}
#endif