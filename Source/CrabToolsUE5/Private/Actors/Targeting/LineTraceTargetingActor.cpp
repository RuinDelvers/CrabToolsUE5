#include "Actors/Targeting/LineTraceTargetingActor.h"
#include "Actors/Targeting/ITargeter.h"

void ALineTraceTargetingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		auto CheckActor = Result.GetActor();
		FTargetingData InData;

		InData.TargetActor = CheckActor;
		InData.TargetLocation = Result.ImpactPoint;
		InData.TargetNormal = Result.ImpactNormal;

		this->UpdateTraces(InData);
	}
	else
	{
		this->InvalidateTargetData();
	}
}
