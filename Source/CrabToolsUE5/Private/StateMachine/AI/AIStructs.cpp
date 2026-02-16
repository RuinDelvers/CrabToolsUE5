#include "StateMachine/AI/AIStructs.h"
#include "NavFilters/NavigationQueryFilter.h"


void FMoveToData::SetOverrideLocation(FVector NewLocation)
{
	this->bUseOverrideLocation = true;
	this->OverrideDestinationLocation = NewLocation;
}

void FMoveToData::ResetGoal()
{
	this->DestinationLocation = FVector::ZeroVector;
	this->DestinationActor = nullptr;
	this->Result.MoveId = FAIRequestID::InvalidRequest;
	this->bUseOverrideLocation = false;
}


bool FMoveToData::ResumeMove(AAIController* Ctrl) const
{
	check(Ctrl);
	
	switch (Ctrl->GetMoveStatus())
	{
		case EPathFollowingStatus::Idle:
			return false;
		case EPathFollowingStatus::Moving:
			return true;
		case EPathFollowingStatus::Paused:
			return Ctrl->ResumeMove(this->Result.MoveId);
		case EPathFollowingStatus::Waiting:
			return false;
		default:
			return false;
	}
}

bool FMoveToData::PauseMove(AAIController* Ctrl) const
{
	check(Ctrl);
	switch (Ctrl->GetMoveStatus())
	{
	case EPathFollowingStatus::Idle:
		return true;
	case EPathFollowingStatus::Moving:
		return Ctrl->PauseMove(this->Result.MoveId);
	case EPathFollowingStatus::Paused:
		return true;
	case EPathFollowingStatus::Waiting:
		return true;
	default:
		return true;
	}
}

bool FMoveToData::MakeRequest(AAIController* Ctrl)
{
	check(Ctrl);

	if (this->DestinationActor || this->bUseLocationIfNoGoal || this->bUseOverrideLocation)
	{
		FAIMoveRequest Request;

		Request.SetUsePathfinding(this->bUsePathfinding);
		Request.SetAllowPartialPath(this->bAllowPartialPaths);
		Request.SetNavigationFilter(*this->FilterClass ? this->FilterClass : Ctrl->GetDefaultNavigationFilterClass());
		Request.SetAcceptanceRadius(this->AcceptanceRadius);
		Request.SetReachTestIncludesAgentRadius(this->bStopOnOverlap);
		Request.SetCanStrafe(this->bCanStrafe);

		if (this->DestinationActor && !this->bUseOverrideLocation)
		{
			Request.SetGoalActor(this->DestinationActor);		
		}
		else
		{
			Request.SetGoalLocation(
				this->bUseOverrideLocation
				? this->OverrideDestinationLocation
				: this->DestinationLocation);
		}

		this->Result = Ctrl->MoveTo(Request, &this->SavedPath);
		return true;
	}
	else
	{
		this->Result = FPathFollowingRequestResult();
		return false;
	}
}