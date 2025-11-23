#pragma once

#include "Components/TimelineComponent.h"
#include "Navigation/NavLinkProxy.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "DoorActor.generated.h"

UENUM(BlueprintType)
enum class EDoorState: uint8
{
	OPEN    UMETA(DisplayName = "Open"),
	OPENING UMETA(DisplayName = "Opening"),
	CLOSED  UMETA(DisplayName = "Closed"),
	CLOSING UMETA(DisplayName = "Closing"),
};

UCLASS(ClassGroup = (General), meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UDoorActorMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

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

	UPROPERTY(Transient)
	TArray<TObjectPtr<UDoorActorMeshComponent>> Components;

	UPROPERTY(BlueprintReadOnly, Category="DoorActor", meta=(AllowPrivateAccess))
	EDoorState State = EDoorState::CLOSED;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorActor")
	TObjectPtr<UCurveFloat> MovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DoorActor")
	float PlayRate;

	float CurrentAlpha;
	FVector Axis;
	FQuat BaseRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DoorActor")
	TObjectPtr<UTimelineComponent> MovementTimeline;
	FOnTimelineFloat MovementTrack;
	FOnTimelineEvent EndMovementTrack;	

public:

	UPROPERTY(BlueprintReadWrite, Category = "DoorActor")
	FFinishMovement OnFinishMovementEvent;


public:

	ADoorActor();

	EDoorState GetDoorState() const { return this->State; }

	UFUNCTION(BlueprintCallable, Category="DoorActor")
	void ActivateDoor(bool OpenQ);

	UFUNCTION(BlueprintCallable, Category = "DoorActor")
	void SetPlayRate(float NewPlayRate);
	
	UFUNCTION(BlueprintCallable, Category = "DoorActor")
	FORCEINLINE void OpenDoor() { ActivateDoor(true); }

	UFUNCTION(BlueprintCallable, Category = "DoorActor")
	FORCEINLINE void CloseDoor() { ActivateDoor(false); }

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "DoorActor")
	void ToggleDoorEditor();

	UFUNCTION(BlueprintCallable, Category = "DoorActor")
	void ToggleDoor();

protected:

	void OnDoorStateChanged(EDoorState NewState);

	UFUNCTION(BlueprintNativeEvent, Category="DoorActor", meta=(DisplayName="OnDoorStateChanged"))
	void OnDoorStateChanged_Inner();
	virtual void OnDoorStateChanged_Inner_Implementation() {}

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdatePosition(float alpha);

	UFUNCTION()
	void FinishMovement();

private:

	TArray<TObjectPtr<UDoorActorMeshComponent>>& GetDoorComponents();
};
