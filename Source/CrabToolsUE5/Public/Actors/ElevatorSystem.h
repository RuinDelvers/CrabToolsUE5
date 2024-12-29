#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Actors/Paths/PatrolPath.h"
#include "ElevatorSystem.generated.h"

namespace ElevatorSystem::Constants
{
	constexpr int NULL_TARGET = -1;
}


UCLASS()
class AElevatorSystem : public AActor
{
	GENERATED_BODY()

	TObjectPtr<UTimelineComponent> MovementTimeline;
	FOnTimelineFloat MovementTrack;
	FOnTimelineEvent EndMovementTrack;

	UPROPERTY(EditAnywhere, Category = "ElevatorSystem",
		meta=(AllowPrivateAccess))
	TObjectPtr<UCurveFloat> MovementCurve;

	UPROPERTY(EditAnywhere, Category = "ElevatorSystem",
		meta = (AllowPrivateAccess, ShowOnlyInnerProperties))
	FPatrolPathState State;

	/* Used when traversing multiple points. */
	int TargetIndex = ElevatorSystem::Constants::NULL_TARGET;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FArrivedEvent, int, Index);
	UPROPERTY(BlueprintAssignable, Category="ElevatorSystem")
	FArrivedEvent OnArrivedEvent;

public:	
	// Sets default values for this actor's properties
	AElevatorSystem();

	FVector GetMovementStart() const;
	FVector GetMovementDirection() const;

	/* Move the elevetor to the next position. */
	UFUNCTION(BlueprintCallable, Category = "ElevatorSystem")
	virtual void Step();	

	UFUNCTION(BlueprintCallable, Category = "ElevatorSystem")
	void GoToIndex(int i);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ElevatorSystem")
	void PostFinishCallback();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	void StartMovement();

	UFUNCTION()
	virtual void UpdatePosition(float alpha);

	UFUNCTION()
	void FinishedMovement();
};
