#pragma once

#include "Components/ActorComponent.h"
#include "Actors/Paths/PatrolPath.h"
#include "PatrolPathFollowingComponent.generated.h"


UCLASS(ClassGroup = (General), meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UPatrolPathFollowingComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category="Path", meta=(ShowOnlyInnerProperties))
	FPatrolPathState State;

	UPROPERTY(EditAnywhere, Category = "Path", meta = (ShowOnlyInnerProperties))
	TMap<FName, FPatrolPathState> States;

public:

	bool HasPathState(FName PathKey) const {  return PathKey.IsNone() || this->States.Contains(PathKey); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Path")
	FPatrolPathState& GetPathState(FName PathKey=NAME_None);
};