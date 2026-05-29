#include "Actors/Targeting/MouseTraceTargetingActor.h"
#include "Actors/Targeting/ITargeter.h"

void AMouseTraceTargetingActor::HandleTrace_Implementation(FTargetingData& OutData)
{
	if (auto PlayerController = Cast<APlayerController>(Cast<APawn>(this->GetUsingActor())->GetController())) 
	{
		FVector MousePos;
		FVector MouseDir;
		PlayerController->DeprojectMousePositionToWorld(MousePos, MouseDir);

		FVector EndPoint = MousePos + MouseDir * this->TraceDistance;

		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);

		TArray<AActor*> IgnoredActors;
		this->IgnoreActors(IgnoredActors);
		TraceParams.AddIgnoredActors(IgnoredActors);

		FHitResult Result(ForceInit);

		bool FoundTarget = this->GetWorld()->LineTraceSingleByChannel(
			Result,
			MousePos,
			EndPoint,
			this->TraceChannel,
			TraceParams);

		#if WITH_EDITORONLY_DATA
				if (this->bDrawDebug)
				{
					DrawDebugLine(this->GetWorld(), MousePos, EndPoint, FColor(1, 1, 1), false, 0.1f, 0, 5.0f);
				}
		#endif //WITH_EDITORONLY_DATA

		if (FoundTarget)
		{
			OutData.TargetActor = Result.GetActor();
			OutData.TargetLocation = Result.ImpactPoint;
			OutData.TargetNormal = Result.ImpactNormal;
			OutData.SourcePoint = this->GetActorLocation();
		}
	}	
}
