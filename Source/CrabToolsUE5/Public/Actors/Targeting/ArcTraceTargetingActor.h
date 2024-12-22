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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, ExposeOnSpawn = true));
	float MaxHeight;

public:

	AArcTraceTargetingActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|ArcTrace")
	void OnTooHigh();
	virtual void OnTooHigh_Implementation() {}

	virtual void OnUpdateTraces_Implementation() override;

	bool IsTooHigh() const;
};
