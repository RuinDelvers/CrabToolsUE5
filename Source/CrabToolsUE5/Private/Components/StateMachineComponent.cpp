// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StateMachineComponent.h"
#include "Engine.h"

// Sets default values for this component's properties
UStateMachineComponent::UStateMachineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


// Called when the game starts
void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	if (this->Machine)
	{
		//this->Machine->OnStateChanged.AddDynamic(this, &UStateMachineComponent::Statechanged);
		this->Machine->OnTickRequirementUpdated.AddDynamic(this, &UStateMachineComponent::TickUpdated);
		this->Machine->Initialize(this->GetOwner());		
	}
}


// Called every frame
void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (this->Machine) {
		this->Machine->Tick(DeltaTime);
	}
}


void UStateMachineComponent::Event_Direct(FName EName) {
	if (this->HasMachine()) {
		this->Machine->SendEvent(EName);
	}
}

void UStateMachineComponent::EventWithData_Direct(FName EName, UObject* Data) {
	if (this->HasMachine()) {
		this->Machine->SendEventWithData(EName, Data);
	}
}

FName UStateMachineComponent::CurrentStateName() {
	return this->Machine->GetCurrentStateName();
}

bool UStateMachineComponent::HasMachine() {
	return this->Machine != nullptr;
}

void UStateMachineComponent::TickUpdated(bool NeedsTick)
{
	this->SetComponentTickEnabled(NeedsTick);
}