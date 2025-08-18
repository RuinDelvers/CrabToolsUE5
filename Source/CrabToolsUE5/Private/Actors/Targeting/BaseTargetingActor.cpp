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

void ABaseTargetingActor::AddDestroyedListener_Implementation(const FTargetingUpdated& Callback)
{
	this->OnDestroyed.Add(Callback);
}

void ABaseTargetingActor::AddDisabledListener_Implementation(const FTargetingUpdated& Callback)
{
	this->OnEnabledUpdated.Add(Callback);
}

void ABaseTargetingActor::AddValidationListener_Implementation(const FValidateTargetingSingle& Callback)
{
	this->OnValidateTargeting.Add(Callback);
}

void ABaseTargetingActor::SetEnabled_Implementation(bool bNewEnabled)
{
	this->bEnabled = bEnabled;
	this->OnEnabledUpdated.Broadcast(this);
}

void ABaseTargetingActor::Confirm_Implementation()
{
	TScriptInterface<ITargetingControllerInterface> Targeter(this);
	this->OnConfirmTargets.Broadcast(Targeter);
}

void ABaseTargetingActor::GetTargetData_Implementation(TArray<FTargetingData>& OutData) const
{
	OutData.Append(this->Data);
}

void ABaseTargetingActor::PushTargetData(const FTargetingData& InData)
{
	this->Data.Add(InData);
}

void ABaseTargetingActor::PopTarget_Implementation()
{
	this->Data.Pop();
}