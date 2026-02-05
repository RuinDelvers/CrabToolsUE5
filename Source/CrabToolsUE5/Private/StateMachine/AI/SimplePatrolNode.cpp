#include "StateMachine/AI/SimplePatrolNode.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/Events.h"


#define LOCTEXT_NAMESPACE "AISimplePatrolNode"

FPatrolPathState TempState;

UAISimplePatrolNode::UAISimplePatrolNode()
{
	this->AddEmittedEvent(Events::AI::ARRIVE);
	this->AddEmittedEvent(Events::AI::LOST);
	
	this->ComponentPattern = CreateDefaultSubobject<UActorComponentPatternRef>(TEXT("ComponentPattern"));
}

void UAISimplePatrolNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();
}

void UAISimplePatrolNode::SetActive_Inner_Implementation(bool bNewActive)
{
	if (bNewActive)
	{
		this->GetAIController()->ResumeMove(this->Result.MoveId);
	}
	else
	{
		this->GetAIController()->PauseMove(this->Result.MoveId);
	}
}

void UAISimplePatrolNode::Enter_Inner_Implementation()
{
	// If the path doesn't exist, then don't attempt work.
	if (!this->GetPath())
	{
		this->EmitEvent(Events::AI::LOST);
	}
	// If the path key exists, make sure it has it. 
	else if (this->PathKey.IsNone() && !this->GetPath()->HasPathState(this->PathKey))
	{
		this->EmitEvent(Events::AI::LOST);
	}
	else
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
			auto& State = this->GetState();
			State.Reset();
		}

		this->BindCallback();
		this->MoveToNext();
	}
}

UPatrolPathFollowingComponent* UAISimplePatrolNode::GetPath() const
{
	if (this->bCacheComponent)
	{
		if (!this->CachedComp)
		{
			this->CachedComp = this->ComponentPattern->FindComponentByClass<UPatrolPathFollowingComponent>(this->GetActorOwner());
		}

		return this->CachedComp;
	}
	else
	{
		return this->ComponentPattern->FindComponentByClass<UPatrolPathFollowingComponent>(this->GetActorOwner());
	}
}

FPatrolPathState& UAISimplePatrolNode::GetState() const
{
	if (this->bCacheComponent)
	{
		if (!this->CachedComp)
		{
			this->CachedComp = this->ComponentPattern->FindComponentByClass<UPatrolPathFollowingComponent>(this->GetActorOwner());
		}

		return this->CachedComp->GetPathState(PathKey);
	}
	else
	{
		return this->ComponentPattern->FindComponentByClass<UPatrolPathFollowingComponent>(this->GetActorOwner())->GetPathState(PathKey);
	}		
}

void UAISimplePatrolNode::Exit_Inner_Implementation()
{
	this->UnbindCallback();

	this->GetAIController()->StopMovement();
}

void UAISimplePatrolNode::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MoveResult)
{
	auto& State = this->GetState();
	State.Step();

	switch (MoveResult)
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
	auto& State = this->GetState();
	auto Ctrl = this->GetAIController();

	FVector Goal = State.Point();

	this->RecurseGuard += 1;

	if (this->RecurseGuard > State.Points())
	{
		// If we've recursed too many times, then remove the call back.
		this->GetAIController()->ReceiveMoveCompleted.RemoveAll(this);
		this->EmitEvent(Events::AI::LOST);
	}
	else
	{
		this->Result = Ctrl->MoveTo(Goal);
		this->RecurseGuard = 0;
	}
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