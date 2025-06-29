#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Navigation/NavLinkProxy.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "DoorActor.generated.h"

UCLASS(ClassGroup = (General), meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UDoorActorMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

private:

	static TWeakObjectPtr<UCurveVector> DefaultRotationCurve;
	static TWeakObjectPtr<UCurveVector> DefaultTranslationCurve;

private:
	
	UPROPERTY(EditAnywhere, Category="Curves", meta=(AllowPrivateAccess))
	TObjectPtr<UCurveVector> Rotation;

	UPROPERTY(EditAnywhere, Category = "Curves", meta = (AllowPrivateAccess))
	TObjectPtr<UCurveVector> Translation;

	UPROPERTY()
	FVector InitPosition;

	UPROPERTY()
	FRotator InitRotation;

public:

	static UCurveVector* GetDefaultRotationCurve();
	static UCurveVector* GetDefaultTranslationCurve();

public:

	UDoorActorMeshComponent();

	virtual void BeginPlay() override;

	void Update(float Alpha);
};

UCLASS(Blueprintable, meta=(PrioritizeCategories="DoorActor"))
class ADoorActor : public ANavLinkProxy
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFinishMovement);

private:

	/* Dynamically created curve asset that is used as a default curve. */
	static TWeakObjectPtr<UCurveFloat> DefaultForwardCurve;
	static TWeakObjectPtr<UCurveFloat> DefaultReverseCurve;

public:

	static UCurveFloat* GetDefaultForwardCurve();
	static UCurveFloat* GetDefaultReverseCurve();

private:

	TArray<TObjectPtr<UDoorActorMeshComponent>> Components;

public:	
	
	ADoorActor();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorActor")
	TObjectPtr<UCurveFloat> MovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorActor")
	float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorActor",
		meta=(ClampMin="-360", ClampMax="360", UIMin="-360", UIMax="360.0"))
	float AngleDelta;
	float CurrentAlpha;
	FVector Axis;
	FQuat BaseRotation;

	UPROPERTY(BlueprintReadOnly, Category="DoorActor")
	TObjectPtr<UTimelineComponent> MovementTimeline;
	FOnTimelineFloat MovementTrack;
	FOnTimelineEvent EndMovementTrack;	

public:

	UPROPERTY(BlueprintReadWrite, Category = "DoorActor")
	FFinishMovement OnFinishMovementEvent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdatePosition(float alpha);

	UFUNCTION()
	void FinishMovement();

public:

	UFUNCTION(BlueprintCallable, Category="DoorActor")
	void ActivateDoor(bool OpenQ);

	UFUNCTION(BlueprintCallable, Category = "DoorActor")
	void SetPlayRate(float NewPlayRate);
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "DoorActor")
	FORCEINLINE void OpenDoor() { ActivateDoor(true); }

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "DoorActor")
	FORCEINLINE void CloseDoor() { ActivateDoor(false); }
};
