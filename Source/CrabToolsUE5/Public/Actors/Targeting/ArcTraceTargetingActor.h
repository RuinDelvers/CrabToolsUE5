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
	float HeightAdjust = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting|ArcTrace",
		meta = (AllowPrivateAccess))
	int SampleSize = 10;

public:

	AArcTraceTargetingActor();

	virtual void HandleTrace_Implementation() override;

	UFUNCTION(BlueprintCallable, Category="Targeting|ArcTrace")
	void SetSplineEndPoint(FVector Target);
};
