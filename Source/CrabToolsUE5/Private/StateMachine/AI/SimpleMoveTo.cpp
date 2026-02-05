#include "StateMachine/AI/SimpleMoveTo.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/Events.h"
#include "StateMachine/AI/AIStructs.h"


UAISimpleMoveToNode::UAISimpleMoveToNode(): MovementResult(EPathFollowingResult::Aborted)
{
	this->AddEmittedEvent(Events::AI::ARRIVE);
	this->AddEmittedEvent(Events::AI::LOST);

	this->Property = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("MoveToTarget"));
}

void UAISimpleMoveToNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	this->Property->Initialize();

	check(this->GetAIController());
}

void UAISimpleMoveToNode::Exit_Inner_Implementation()
{
	this->UnbindCallback();
	this->StopMovement();
	this->MoveData.ResetGoal();
}

void UAISimpleMoveToNode::Enter_Inner_Implementation()
{
	this->BindCallback();

	if (this->MoveData.bResumePreviousPath)
	{
		auto ResumeResult = this->GetAIController()->ResumeMove(this->Result.MoveId);

		if (Result.Code == EPathFollowingRequestResult::RequestSuccessful)
		{
			return;
		}
	}

	if (this->Property->IsBound())
	{
		bool bFoundData = false;
		auto& Value = this->Property->GetStruct<FMoveToData>(bFoundData);

		if (bFoundData)
		{
			this->Result = Value.MakeRequest(this->GetAIController());

			if (this->Result.Code == EPathFollowingRequestResult::Failed)
			{
				this->EmitEvent(Events::AI::LOST);
			}
		}
	}	
	else
	{
		this->MoveTo();
	}
}

void UAISimpleMoveToNode::SetActive_Inner_Implementation(bool bNewActive)
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

void UAISimpleMoveToNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	this->MoveData.DestinationActor = Cast<AActor>(Data);	
	this->Enter_Inner();
}

void UAISimpleMoveToNode::MoveTo()
{
	this->Result = this->MoveData.MakeRequest(this->GetAIController());

	if (this->Result.Code == EPathFollowingRequestResult::Failed)
	{
		this->EmitEvent(Events::AI::LOST);
	}
}


void UAISimpleMoveToNode::SetOverrideLocation(FVector Location)
{
	this->MoveData.DestinationLocation = Location;
	this->MoveData.bUseOverrideLocation = true;
}

void UAISimpleMoveToNode::StopMovement()
{
	this->GetAIController()->StopMovement();
}

void UAISimpleMoveToNode::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MoveResult)
{
	this->MovementResult = MoveResult;

	switch (MoveResult)
	{
		case EPathFollowingResult::Success: this->EmitEvent(Events::AI::ARRIVE); break;
		case EPathFollowingResult::Aborted: this->EmitEvent(Events::AI::ARRIVE); break;
		default:this->EmitEvent(Events::AI::LOST);
	}
}

void UAISimpleMoveToNode::BindCallback()
{
	auto CtrlQ = this->GetAIController();	
	CtrlQ->ReceiveMoveCompleted.AddDynamic(this, &UAISimpleMoveToNode::OnMoveCompleted);
}

void UAISimpleMoveToNode::UnbindCallback()
{
	auto CtrlQ = this->GetAIController();
	CtrlQ->ReceiveMoveCompleted.RemoveAll(this);
}

void UAISimpleMoveToNode::EventNotify_PauseMovement(FName InEvent)
{
	this->GetAIController()->PauseMove(this->Result.MoveId);
}

void UAISimpleMoveToNode::EventNotify_ResumeMovement(FName InEvent)
{
	this->GetAIController()->ResumeMove(this->Result.MoveId);
}

#if WITH_EDITOR
void UAISimpleMoveToNode::PostLinkerChange()
{
	Super::PostLinkerChange();
}
#endif