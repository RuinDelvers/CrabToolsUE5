#include "Actors/Targeting/BaseTargetingActor.h"

ABaseTargetingActor::ABaseTargetingActor()
{
	
}

void ABaseTargetingActor::BeginPlay()
{
	Super::BeginPlay();
	ITargetingControllerInterface::Execute_Initialize(this);
}

void ABaseTargetingActor::Initialize_Implementation()
{
	FAttachmentTransformRules;
	this->AttachToActor(this->UsingActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ABaseTargetingActor::AddListener_Implementation(const FConfirmTargetsSingle& Callback)
{
	this->OnConfirmTargets.Add(Callback);
}

void ABaseTargetingActor::Confirm_Implementation()
{
	this->OnConfirmTargets.Broadcast();
}