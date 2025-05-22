#pragma once

#include "CoreMinimal.h"
#include "Actors/Targeting/BaseTargetingActor.h"
#include "BaseTraceTargetingActor.generated.h"

UCLASS(Abstract, Blueprintable)
class ABaseTraceTargetingActor : public ABaseTargetingActor
{
	GENERATED_BODY()

protected:

	/* Whether or not to draw debug lines for the trace. */
	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug",
			meta = (AllowPrivateAccess))
		bool bDrawDebug = false;
	#endif // WITH_EDITORONLY_DATA

	/* Scale the direction to the target by the given percentage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting|Trace",
		meta = (AllowPrivateAccess, Units="Percent"))
	float CorrectionFactor = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting|Trace",
		meta = (AllowPrivateAccess))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Trace",
		meta = (AllowPrivateAccess))
	bool bIgnoreSelf = true;

	TArray<AActor*> AddedActors;
	TArray<FVector> AddedPoints;

	/* The actor traced for targeting. */
	UPROPERTY(BlueprintreadOnly, Category = "Targeting|Trace")
	TObjectPtr<AActor> TracedActor;

	/* The impact point that was traced by this targeting actor. */
	UPROPERTY(BlueprintreadOnly, Category = "Targeting|Trace")
	FVector TracedLocation;

	/* The point received via the GetEndPoint interface call. */
	UPROPERTY(BlueprintreadOnly, Category = "Targeting|Trace")
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, ExposeOnSpawn = true));
	float Range;

public:

	ABaseTraceTargetingActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Targeting|Trace", meta=(HideSelfPin))
	FVector GetEndPoint() const { return this->TracedLocation; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Targeting|Trace", meta = (HideSelfPin))
	AActor* GetEndPointActor() const { return this->TracedActor.Get(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(HideSelfPin))
	int GetTargetCount() const { return this->AddedActors.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Targeting|Trace")
	void UpdateTraces(AActor* CheckedActor, FVector Location);

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Trace")
	void OnUpdateTraces();
	virtual void OnUpdateTraces_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category="Targeting|Trace")
	bool IsValidTarget(AActor* CheckedActor);
	virtual bool IsValidTarget_Implementation(AActor* CheckedActor);

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Trace")
	bool IsValidPoint(FVector Point);
	virtual bool IsValidPoint_Implementation(FVector Point);

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Trace")
	void OnTooFar();
	virtual void OnTooFar_Implementation() {}

	UFUNCTION(BlueprintCallable, Category="Targeting|Trace")
	bool IsTooFar() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Trace")
	void OnInvalidTarget();
	virtual void OnInvalidTarget_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Trace")
	void IgnoreActors(TArray<AActor*>& IgnoredActors);
	virtual void IgnoreActors_Implementation(TArray<AActor*>& IgnoredActors);

	virtual void PushTarget_Implementation() override;
	virtual void PopTarget_Implementation() override;
	virtual void GetTargets_Implementation(TArray<AActor*>& Actors) const { Actors.Append(this->AddedActors); };
	virtual void GetTargetPoints_Implementation(TArray<FVector>& Points) const override { Points.Append(this->AddedPoints); }

	UFUNCTION(BlueprintNativeEvent, Category="Target|Trace")
	FVector GetTraceBase() const;
	FVector GetTraceBase_Implementation() const { return this->GetActorLocation(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetTargetEndPoint() const { return this->TargetLocation; }

private:

	FORCEINLINE void InvalidateTargetData();
};
