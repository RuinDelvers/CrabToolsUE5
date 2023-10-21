// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ProcStateMachineComponent.h"

// Sets default values for this component's properties
UProcStateMachineComponent::UProcStateMachineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}


// Called when the game starts
void UProcStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();
	if (this->MachineClass.Get() != nullptr) {
		this->Machine = NewObject<UProcStateMachine>(
			this, 
			this->MachineClass,
			NAME_None,
			RF_NoFlags, 
			this->MachineClass.GetDefaultObject(), 
			true);
		this->Machine->Initialize(this->GetOwner());
	}
}


// Called every frame
void UProcStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (this->Machine != nullptr) {
		this->Machine->Tick(DeltaTime);
	}
}
