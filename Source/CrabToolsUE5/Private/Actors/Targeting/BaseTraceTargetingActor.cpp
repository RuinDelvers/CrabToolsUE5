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
	this->TracedTarget = FTargetingData();
}

void ABaseTraceTargetingActor::PushTarget_Implementation()
{
	if (IsValid(this->TracedTarget.TargetActor))
	{
		this->PushTargetData(this->TracedTarget);
	}
}

void ABaseTraceTargetingActor::UpdateTraces(const FTargetingData& InData)
{
	this->TracedTarget = InData;

	FText Reason;
	bool IsValid = ITargetingControllerInterface::Execute_Validate(this, Reason);

	if (!IsValid)
	{
		this->InvalidateTargetData();
	}

	this->OnUpdateTraces();
	this->OnValidateTargeting.Broadcast(IsValid, Reason);
}

bool ABaseTraceTargetingActor::IsTooFar() const
{
	return (this->TracedTarget.TargetLocation - this->GetActorLocation()).Length() > this->Range;
}

bool ABaseTraceTargetingActor::IsValidTarget_Implementation() const
{
	return IsValid(this->TracedTarget.TargetActor);
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