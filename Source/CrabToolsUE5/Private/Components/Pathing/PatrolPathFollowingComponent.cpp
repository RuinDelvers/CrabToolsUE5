#include "Components/Pathing//PatrolPathFollowingComponent.h"

FPatrolPathState& UPatrolPathFollowingComponent::GetPathState(FName PathKey)
{
	if (this->States.Contains(PathKey))
	{
		return this->States[PathKey];
	}
	else
	{
		return this->State;
	}
}