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

	UPROPERTY(BlueprintreadOnly, Category = "Targeting|Trace")
	FTargetingData TracedTarget;

	/* The point received via the GetEndPoint interface call. */
	UPROPERTY(BlueprintreadOnly, Category = "Targeting|Trace")
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, ExposeOnSpawn = true));
	float Range;

public:

	ABaseTraceTargetingActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Targeting|Trace", meta=(HideSelfPin))
	FVector GetEndPoint() const { return this->TracedTarget.TargetLocation; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Targeting|Trace", meta = (HideSelfPin))
	AActor* GetEndPointActor() const { return this->TracedTarget.TargetActor; }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(HideSelfPin))
	int GetTargetCount() const { return this->Data.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Targeting|Trace")
	void UpdateTraces(const FTargetingData& Data);

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Trace")
	void OnUpdateTraces();
	virtual void OnUpdateTraces_Implementation() {}

	UFUNCTION(BlueprintCallable, Category="Targeting|Trace")
	bool IsTooFar() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Trace")
	void IgnoreActors(TArray<AActor*>& IgnoredActors);
	virtual void IgnoreActors_Implementation(TArray<AActor*>& IgnoredActors);

	virtual void PushTarget_Implementation() override;
	virtual bool Validate_Implementation(FText& Reason) const override;

	UFUNCTION(BlueprintNativeEvent, Category="Target|Trace")
	FVector GetTraceBase() const;
	FVector GetTraceBase_Implementation() const { return this->GetActorLocation(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetTargetEndPoint() const { return this->TargetLocation; }

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Trace")
	bool IsValidTarget() const;
	virtual bool IsValidTarget_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Trace")
	bool IsValidPoint() const;
	virtual bool IsValidPoint_Implementation() const;

protected:

	

	//UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Trace")
	//void OnTooFar() const;
	//virtual void OnTooFar_Implementation() const {}

private:

	FORCEINLINE void InvalidateTargetData();
};
