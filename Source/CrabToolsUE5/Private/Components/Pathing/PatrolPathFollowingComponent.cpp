#include "Components/Pathing//PatrolPathFollowingComponent.h"

FPatrolPathState& UPatrolPathFollowingComponent::GetPathState(FName PathKey)
{
	if (PathKey.IsNone())
	{
		return this->State;
	}
	else
	{
		if (this->States.Contains(PathKey))
		{
			return this->States[PathKey];
		}
		else
		{
			ensureMsgf(false, TEXT("A non-None path key has been passed, but is not contained in the patrol path. (%s)"), *PathKey.ToString());
			return this->State;
		}
	}
}