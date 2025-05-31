#include "Actors/Targeting/BaseTraceTargetingActor.h"
#include "Actors/Targeting/ITargeter.h"

ABaseTraceTargetingActor::ABaseTraceTargetingActor() : Range(std::numeric_limits<float>::infinity())
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
	this->TracedActor = nullptr;
	this->TracedLocation = this->GetActorLocation();
}

void ABaseTraceTargetingActor::PushTarget_Implementation()
{
	if (IsValid(this->TracedActor.Get()))
	{
		this->AddedActors.Add(this->TracedActor.Get());
		this->AddedPoints.Add(this->TracedLocation);
	}
}

void ABaseTraceTargetingActor::UpdateTraces(AActor* CheckedActor, FVector Location)
{
	this->TracedActor = CheckedActor;
	this->TracedLocation = Location;

	FText Reason;
	bool IsValid = ITargetingControllerInterface::Execute_Validate(this, Reason);

	if (!IsValid)
	{
		this->TracedActor = nullptr;
		this->TracedLocation = FVector::Zero();
	}

	this->OnUpdateTraces();
	this->OnValidateTargeting.Broadcast(IsValid, Reason);
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

bool ABaseTraceTargetingActor::IsValidTarget_Implementation() const
{
	return IsValid(this->TracedActor);
}

bool ABaseTraceTargetingActor::IsValidPoint_Implementation() const
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

bool ABaseTraceTargetingActor::Validate_Implementation(FText& Reason) const
{
	bool ValidTarget = this->IsValidTarget();
	bool ValidPoint = this->IsValidPoint();

	FText Summary = NSLOCTEXT("", "", "{ValidTarget}{ValidPoint}");
	FFormatNamedArguments Args;

	if (!ValidTarget)
	{
		Args.Add(TEXT("ValidTarget"), FText::FromString("Invalid Target"));
	}
	else
	{
		Args.Add(TEXT("ValidTarget"), FText::FromString(""));
	}

	if (!ValidPoint)
	{
		if (ValidTarget)
		{
			if (this->IsTooFar())
			{
				Args.Add(TEXT("ValidPoint"), FText::FromString("Out of range."));
			}			
		}
		else
		{
			if (this->IsTooFar())
			{
				Args.Add(TEXT("ValidPoint"), FText::FromString("\nOut of range."));
			}
		}
	}
	else
	{
		Args.Add(TEXT("ValidPoint"), FText::FromString(""));
	}
	
	Reason = FText::Format(Summary, Args);
	return ValidTarget && ValidPoint;
}