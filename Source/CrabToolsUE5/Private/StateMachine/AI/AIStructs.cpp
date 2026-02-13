#include "StateMachine/AI/AIStructs.h"



void FMoveToData::ResetGoal()
{
	this->DestinationLocation = FVector::ZeroVector;
	this->DestinationActor = nullptr;
	this->Result.MoveId = FAIRequestID::InvalidRequest;
	this->bUseOverrideLocation = false;
}

void FMoveToData::ClearIfNoCache()
{
	if (!this->bResumePreviousPath)
	{
		this->Result = FPathFollowingRequestResult();
	}
}

bool FMoveToData::ResumeMove(AAIController* Ctrl) const
{
	check(Ctrl);

	return Ctrl->ResumeMove(this->Result.MoveId);
}

bool FMoveToData::PauseMove(AAIController* Ctrl) const
{
	return Ctrl->PauseMove(this->Result.MoveId);
}

void FMoveToData::MakeRequest(AAIController* Ctrl)
{
	check(Ctrl);

	if (this->bResumePreviousPath)
	{
		if (Ctrl->ResumeMove(this->Result.MoveId))
		{
			return;
		}
	}

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
		this->Result = Ctrl->MoveTo(Request, &this->SavedPath);
	}
	else
	{
		this->Result = FPathFollowingRequestResult();
	}
}