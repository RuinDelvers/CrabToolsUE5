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
		this->Machine->OnTickRequirementUpdated.AddUniqueDynamic(this, &UStateMachineComponent::TickUpdated);
		this->Machine->Initialize(this->GetOwner());		
	}
}

void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (this->Machine)
	{
		this->Machine->Tick(DeltaTime);
	}
}

void UStateMachineComponent::CreateMachine(TSubclassOf<UStateMachine> MachineClass)
{
	auto NewMachine = NewObject<UStateMachine>(this, MachineClass);

	if (NewMachine)
	{
		if (this->Machine)
		{
			this->Machine->OnTickRequirementUpdated.RemoveAll(this);
		}

		this->Machine = NewMachine;
		this->Machine->OnTickRequirementUpdated.AddUniqueDynamic(this, &UStateMachineComponent::TickUpdated);
		this->Machine->Initialize(this->GetOwner());
	}
}

void UStateMachineComponent::Event_Direct(FName EName)
{
	if (this->HasMachine()) {
		this->Machine->SendEvent(EName);
	}
}

void UStateMachineComponent::EventWithData_Direct(FName EName, UObject* Data)
{
	if (this->HasMachine()) {
		this->Machine->SendEventWithData(EName, Data);
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

void UStateMachineComponent::TickUpdated(bool NeedsTick)
{
	this->SetComponentTickEnabled(NeedsTick);
}

#if WITH_EDITOR
void UStateMachineComponent::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);
}
#endif