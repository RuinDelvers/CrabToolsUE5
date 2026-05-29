#include "Actors/Targeting/LineTraceTargetingActor.h"
#include "Actors/Targeting/ITargeter.h"

void ALineTraceTargetingActor::HandleTrace_Implementation(FTargetingData& OutData)
{
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	
	TArray<AActor*> IgnoredActors;
	this->IgnoreActors(IgnoredActors);
	TraceParams.AddIgnoredActors(IgnoredActors);

	FHitResult Result(ForceInit);

	FVector Base = this->GetTraceBase();	
	FVector FixedTarget = (1 + this->CorrectionFactor/100) * (this->GetTargetEndPoint() - Base) + Base;

	#if WITH_EDITORONLY_DATA
		if (this->bDrawDebug)
		{
			DrawDebugLine(this->GetWorld(), Base, FixedTarget, FColor(1, 1, 1), false, 0.1f, 0, 5.0f);
		}
	#endif //WITH_EDITORONLY_DATA

	bool FoundTarget = this->GetWorld()->LineTraceSingleByChannel(
		Result,
		Base,
		FixedTarget,
		this->TraceChannel,
		TraceParams);

	if (FoundTarget)
	{
		OutData.TargetActor = Result.GetActor();
		OutData.TargetLocation = Result.ImpactPoint;
		OutData.TargetNormal = Result.ImpactNormal;
		OutData.SourcePoint = this->GetActorLocation();
	}
}

void ALineTraceTargetingActor::HandleLineOfSight_Implementation(FTargetingData& OutData)
{
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);

	TArray<AActor*> IgnoredActors = { this->GetUsingActorNative() };
	this->IgnoreActors(IgnoredActors);
	TraceParams.AddIgnoredActors(IgnoredActors);

	FHitResult Result(ForceInit);

	FVector Base = this->GetTraceBase();
	FVector FixedTarget = (1 + this->CorrectionFactor / 100) * (this->GetTargetEndPoint() - Base) + Base;

	#if WITH_EDITORONLY_DATA
		if (this->bDrawDebug)
		{
			DrawDebugLine(this->GetWorld(), Base, FixedTarget, FColor(1, 1, 1), false, 0.1f, 0, 5.0f);
		}
	#endif //WITH_EDITORONLY_DATA

	bool FoundTarget = this->GetWorld()->LineTraceSingleByChannel(
		Result,
		Base,
		FixedTarget,
		this->TraceChannel,
		TraceParams);

	OutData.TargetActor = Result.GetActor();
	OutData.TargetLocation = Result.Location;
	OutData.TargetNormal = Result.Normal;
}
