#include "Actors/Targeting/BaseTraceTargetingActor.h"
#include "Actors/Targeting/ITargeter.h"

ABaseTraceTargetingActor::ABaseTraceTargetingActor()
{
	this->PrimaryActorTick.bCanEverTick = true;
}

void ABaseTraceTargetingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->TargetLocation = ITargeterInterface::Execute_GetEndPoint(this->GetUsingActorNative());
}

void ABaseTraceTargetingActor::InvalidateTargetData()
{
	this->TracedTarget = FTargetingData();
}

void ABaseTraceTargetingActor::PushTarget_Implementation()
{
	if (IsValid(this->TracedTarget.TargetActor))
	{
		ITargetingControllerInterface::Execute_AddTarget(this, this->TracedTarget);
	}
}

void ABaseTraceTargetingActor::UpdateTraces(const FTargetingData& InData)
{
	this->TracedTarget = InData;

	FText Reason;
	bool IsValid = this->ApplyFilters(InData, Reason);

	if (!IsValid)
	{
		this->InvalidateTargetData();
	}

	this->OnValidateTargeting.Broadcast(IsValid, Reason);
}

void ABaseTraceTargetingActor::IgnoreActors_Implementation(TArray<AActor*>& IgnoredActors)
{
	if (this->bIgnoreSelf)
	{
		IgnoredActors.Add(this->GetUsingActorNative());
	}
}
