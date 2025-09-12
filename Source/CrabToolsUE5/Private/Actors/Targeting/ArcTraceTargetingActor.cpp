#include "Actors/Targeting/ArcTraceTargetingActor.h"
#include "Components/SplineComponent.h"

AArcTraceTargetingActor::AArcTraceTargetingActor()
{
	this->PrimaryActorTick.bCanEverTick = true;

	this->PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathDisplay"));

	this->PathSpline->SetClosedLoop(false);
	this->PathSpline->bHiddenInGame = false;
	this->PathSpline->SetVisibility(true);
	this->PathSpline->bDrawDebug = true;
	this->PathSpline->SetSimulatePhysics(false);

	this->SetRootComponent(this->PathSpline);
}

void AArcTraceTargetingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->SetSplineEndPoint(this->GetTargetEndPoint());

	float Delta = this->PathSpline->Duration/this->SampleSize;
	bool bFoundTarget = false;

	TArray<AActor*> IgnoredActors;
	this->IgnoreActors(IgnoredActors);
	
	for (int i = 0; i < this->SampleSize; ++i)
	{
		float t0 = i * Delta;
		float t1 = (i + 1) * Delta;

		auto p1 = this->PathSpline->GetLocationAtTime(t0, ESplineCoordinateSpace::World);
		auto p2 = this->PathSpline->GetLocationAtTime(t1, ESplineCoordinateSpace::World);

		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
		TraceParams.AddIgnoredActors(IgnoredActors);

		FHitResult Result(ForceInit);

		#if WITH_EDITORONLY_DATA
			if (this->bDrawDebug)
			{
				DrawDebugLine(this->GetWorld(), p1, p2, FColor(1, 1, 1), false, 0.1f, 10, 5.0f);
			}
		#endif //WITH_EDITORONLY_DATA

		bFoundTarget = this->GetWorld()->LineTraceSingleByChannel(
			Result,
			p1,
			p1 + (p2 - p1) * (1 + this->CorrectionFactor),
			this->TraceChannel,
			TraceParams);

		if (bFoundTarget)
		{
			auto CheckActor = Result.GetActor();

			FTargetingData InData;
			InData.TargetActor = CheckActor;
			InData.TargetLocation = Result.ImpactPoint;
			InData.TargetNormal = Result.ImpactNormal;

			this->UpdateTraces(InData);

			break;
		}
	}

	if (!bFoundTarget)
	{
		this->InvalidateTargetData();
	}
}


void AArcTraceTargetingActor::SetSplineEndPoint(FVector Target)
{
	auto Base = this->GetActorLocation();

	// Choose a halfway point between.
	auto Midpoint = (Target - Base) / 2 + Base;
	// Adjust the arc to be above both points.
	Midpoint.Z = FMath::Max(Base.Z, Target.Z) + this->HeightAdjust;

	this->PathSpline->SetSplineWorldPoints({ Base, Midpoint, Target });
	this->PathSpline->SetTangentAtSplinePoint(0, (Midpoint - Base).GetSafeNormal(),
		ESplineCoordinateSpace::World, true);
	this->PathSpline->SetTangentAtSplinePoint(0, FVector::Zero(),
		ESplineCoordinateSpace::World, true);
	this->PathSpline->SetTangentAtSplinePoint(2, (Target - Midpoint).GetSafeNormal(),
		ESplineCoordinateSpace::World, true);
}