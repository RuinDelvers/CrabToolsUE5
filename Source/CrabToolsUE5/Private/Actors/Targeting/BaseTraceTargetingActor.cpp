#include "Actors/Targeting/BaseTraceTargetingActor.h"
#include "Actors/Targeting/ITargeter.h"

ABaseTraceTargetingActor::ABaseTraceTargetingActor() : Range(std::numeric_limits<float>::infinity())
{
	this->PrimaryActorTick.bCanEverTick = true;
}

void ABaseTraceTargetingActor::InvalidateTargetData()
{
	this->TracedActor = nullptr;
	this->TracedLocation = this->GetActorLocation();
}

void ABaseTraceTargetingActor::PushTarget_Implementation()
{
	if (this->IsValidTarget(this->TracedActor.Get()))
	{
		this->AddedActors.Add(this->TracedActor.Get());
		this->AddedPoints.Add(this->TracedLocation);
	}
}

void ABaseTraceTargetingActor::UpdateTraces(AActor* CheckedActor, FVector Location)
{
	if (!this->IsValidTarget(CheckedActor) || !this->IsValidPoint(Location))
	{
		this->TracedActor = nullptr;
		this->TracedLocation = FVector::Zero();

		this->OnInvalidTarget();

		if (this->IsTooFar())
		{
			this->OnTooFar();
		}
	}
	else
	{
		this->TracedActor = CheckedActor;
		this->TracedLocation = Location;		
	}

	this->OnUpdateTraces();
}

void ABaseTraceTargetingActor::PopTarget_Implementation()
{
	this->AddedActors.Pop();
	this->AddedPoints.Pop();
}

bool ABaseTraceTargetingActor::IsTooFar() const
{
	return (this->TracedLocation - this->GetActorLocation()).Length() > this->Range;
}

bool ABaseTraceTargetingActor::IsValidTarget_Implementation(AActor* CheckedActor)
{
	return IsValid(CheckedActor);
}

bool ABaseTraceTargetingActor::IsValidPoint_Implementation(FVector Point)
{
	return !this->IsTooFar();
}

void ABaseTraceTargetingActor::IgnoreActors_Implementation(TArray<AActor*>& IgnoredActors)
{
	if (this->bIgnoreSelf)
	{
		IgnoredActors.Add(this->GetOwner());
	}
}