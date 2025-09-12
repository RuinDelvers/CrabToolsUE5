#include "StateMachine/AI/AIStructs.h"



void FMoveToData::ResetGoal()
{
	this->DestinationLocation = FVector::ZeroVector;
	this->DestinationActor = nullptr;

	this->bUseOverrideLocation = false;
}

FPathFollowingRequestResult FMoveToData::MakeRequest(AAIController* Ctrl) const
{
	check(Ctrl);

	if (this->DestinationActor || this->bUseLocationIfNoGoal || this->bUseOverrideLocation)
	{
		FAIMoveRequest Request;

		if (this->DestinationActor && !this->bUseOverrideLocation)
		{
			Request.SetGoalActor(this->DestinationActor);
		}
		else
		{
			Request.SetGoalLocation(this->DestinationLocation);
		}

		Request.SetUsePathfinding(this->bUsePathfinding);

		Ctrl->StopMovement();
		return Ctrl->MoveTo(Request);
	}
	else
	{
		FPathFollowingRequestResult Failure;
		Failure.Code = EPathFollowingRequestResult::Failed;

		return Failure;
	}
}