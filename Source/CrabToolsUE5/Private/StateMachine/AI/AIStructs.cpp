#include "StateMachine/AI/AIStructs.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "NavigationPath.h"


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
		FAIMoveRequest Request = this->ConstructRequest(Ctrl);
		this->Result = Ctrl->MoveTo(Request, &this->SavedPath);
		return true;
	}
	else
	{
		this->Result = FPathFollowingRequestResult();
		return false;
	}
}

bool FMoveToData::MakeRequest(AAIController* Ctrl, UNavigationPath* Path)
{
	check(Ctrl);
	check(Path);

	auto ID = Ctrl->RequestMove(this->ConstructRequest(Ctrl), Path->GetPath());

	if (ID.IsValid())
	{
		this->Result.Code = EPathFollowingRequestResult::RequestSuccessful;
		this->Result.MoveId = ID;

		return true;
	}
	else
	{
		this->Result.Code = EPathFollowingRequestResult::Failed;
		this->Result.MoveId = FAIRequestID::InvalidRequest;

		return false;
	}
}

FAIMoveRequest FMoveToData::ConstructRequest(AAIController* Ctrl) const
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

	return Request;
}
