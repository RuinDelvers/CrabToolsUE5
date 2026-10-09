#pragma once

#include "CoreMinimal.h"
#include "Actors/Targeting/BaseTraceTargetingActor.h"
#include "ArcTraceTargetingActor.generated.h"

UCLASS(Blueprintable)
class AArcTraceTargetingActor : public ABaseTraceTargetingActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Target|ArcTrace", meta=(AllowPrivateAccess))
	TObjectPtr<class USplineComponent> PathSpline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting|ArcTrace",
		meta = (AllowPrivateAccess))
	float HeightAdjust = 0;

	/* The resolution to check for collisions/intersections. This is the number of line traces performed.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting|ArcTrace",
		meta = (AllowPrivateAccess))
	int SampleSize = 10;

	/* The gravity used to compute the spline structure. See FArcPointCurve for details. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting|ArcTrace",
		meta = (AllowPrivateAccess))
	float Gravity = -980;

	/* The gravity used to compute the spline structure. See FArcPointCurve for details. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting|ArcTrace",
		meta = (AllowPrivateAccess))
	float Time = 1.0;

public:

	AArcTraceTargetingActor();

	virtual void HandleTrace_Implementation(FTargetingData& OutData) override;
	virtual void HandleLineOfSight_Implementation(FTargetingData& OutData) override;

	UFUNCTION(BlueprintCallable, Category="Targeting|ArcTrace")
	void SetSplineEndPoint(FVector Target);

private:

	void DoTrace(FTargetingData& OutData, bool IgnoreSelf);
};
