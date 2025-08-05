#include "StateMachine/AI/SimplePatrolNode.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/Events.h"
#include "StateMachine/Logging.h"

#define LOCTEXT_NAMESPACE "AISimplePatrolNode"

UAISimplePatrolNode::UAISimplePatrolNode()
{
	this->AddEmittedEvent(Events::AI::ARRIVE);
	this->AddEmittedEvent(Events::AI::LOST);

	this->Property = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("PatrolPathProperty"));
}

void UAISimplePatrolNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	this->Property->Initialize();

	check(this->GetAIController());
}

void UAISimplePatrolNode::PostTransition_Inner_Implementation()
{
	bool bDoReset = true;

	for (auto& IncState : this->NonResetStates)
	{
		if (IncState.StateName == this->GetMachine()->GetPreviousStateName())
		{
			bDoReset = false;
		}
	}

	if (bDoReset)
	{
		bool bFound = false;
		auto& State = this->Property->GetStruct<FPatrolPathState>(bFound);

		if (bFound) { State.Reset(); }
	}

	this->BindCallback();	
	this->MoveToNext();
}

void UAISimplePatrolNode::Exit_Inner_Implementation()
{
	this->UnbindCallback();

	this->GetAIController()->StopMovement();
}

void UAISimplePatrolNode::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	bool bFound = false;
	auto& State = this->Property->GetStruct<FPatrolPathState>(bFound);
	if (bFound) { State.Step(); }

	switch (Result)
	{
		case EPathFollowingResult::Success:
			this->EmitEvent(Events::AI::ARRIVE);
			if (this->Active())
			{
				this->MoveToNext();
			}
			break;
		case EPathFollowingResult::Blocked:
			this->MoveToNext();
			break;
		default:
			this->EmitEvent(Events::AI::LOST);
			break;
	}

}

void UAISimplePatrolNode::MoveToNext()
{
	bool bFound = false;
	auto& State = this->Property->GetStruct<FPatrolPathState>(bFound);

	if (!bFound) { return; }

	auto Ctrl = this->GetAIController();
	FVector Goal = State.Point();

	this->RecurseGuard += 1;

	if (this->RecurseGuard > State.Points())
	{
		// If we've recursed too many times, then remove the call back.
		this->GetAIController()->ReceiveMoveCompleted.RemoveAll(this);
	}

	Ctrl->MoveToLocation(Goal);

	this->RecurseGuard = 0;
}

void UAISimplePatrolNode::BindCallback()
{
	this->GetAIController()->ReceiveMoveCompleted.AddDynamic(this, &UAISimplePatrolNode::OnMoveCompleted);
}

void UAISimplePatrolNode::UnbindCallback()
{
	if (auto CtrlQ = this->GetAIController())
	{
		CtrlQ->ReceiveMoveCompleted.RemoveAll(this);
	}
}

#if WITH_EDITOR
TArray<FString> UAISimplePatrolNode::GetResetStateOptions() const
{
	auto StateLike = UtilsFunctions::GetOuterAs<IStateLike>(this);

	if (StateLike)
	{
		return StateLike->GetEnterStates();
	}
	else
	{
		return {};
	}
}

void UAISimplePatrolNode::PostLinkerChange()
{
	Super::PostLinkerChange();
}

#endif

#undef LOCTEXT_NAMESPACE