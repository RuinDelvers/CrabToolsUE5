#pragma once

#include "Actors/Targeting/BaseTargetingActor.h"
#include "BaseTraceTargetingActor.generated.h"

class UMouseOverComponent;

UENUM(BlueprintType)
enum class ETraceTargetingPointSource: uint8
{
	TARGETER   UMETA(DisplayName = "Targeter"),
	MOUSE_OVER UMETA(DisplayName = "Mouse Over"),
	CUSTOM     UMETA(DisplayName = "Custom"),
};


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

	UPROPERTY(EditDefaultsOnly, Category = "Targeting|Trace")
	ETraceTargetingPointSource PointSource = ETraceTargetingPointSource::TARGETER;

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

public:

	ABaseTraceTargetingActor();

	virtual void Tick(float DeltaTime) override;
	virtual void Initialize_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Targeting|Trace", meta=(HideSelfPin))
	FVector GetEndPoint() const { return this->TracedTarget.TargetLocation; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Targeting|Trace", meta = (HideSelfPin))
	AActor* GetEndPointActor() const { return this->TracedTarget.TargetActor; }

	UFUNCTION(BlueprintCallable, Category = "Targeting|Trace")
	void UpdateTraces(const FTargetingData& Data);

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Trace")
	void IgnoreActors(TArray<AActor*>& IgnoredActors);
	virtual void IgnoreActors_Implementation(TArray<AActor*>& IgnoredActors);

	virtual void PushTarget_Implementation() override;
	virtual void Confirm_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, Category="Target|Trace")
	FVector GetTraceBase() const;
	FVector GetTraceBase_Implementation() const { return this->GetActorLocation(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Target|Trace")
	FVector GetTargetEndPoint() const { return this->TargetLocation; }

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|Trace")
	void HandleTrace();
	virtual void HandleTrace_Implementation() {}

protected:

	void InvalidateTargetData();

private:

	UFUNCTION()
	void HandleMouseOver(UMouseOverComponent* Comp);
};
