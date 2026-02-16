#include "StateMachine/AI/SimpleMoveTo.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/Events.h"
#include "StateMachine/AI/AIStructs.h"
#include "AI/MovementRequest.h"
#include "StateMachine/Logging.h"


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
	//this->UnbindCallback();
	//this->MoveData.PauseMove(this->GetAIController());
}

void UAISimpleMoveToNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Request = UStateMachinePipedData::FindDataImplementing<UMovementRequestInterface>(Data).GetObject())
	{
		this->MoveToRequest(Request);
	}
	else
	{
		this->MoveToPreset();
	}
}

void UAISimpleMoveToNode::MoveToRequest(UObject* Request)
{
	this->BindCallback();
	bool bRequestSuccessful = false;

	switch (IMovementRequestInterface::Execute_GetRequestType(Request))
	{
		case EAIMovementRequestType::TO_ACTOR:
			{
				auto Actor = IMovementRequestInterface::Execute_GetActor(Request);
				UE_VLOG(this, LogStateMachine, Verbose, TEXT("MovementRequest received for node %s (To Actor: %s) (Location = %s)"),
					*this->GetName(),
					*Actor->GetName(),
					*Actor->GetActorLocation().ToString());
				this->StopMovement();
				this->MoveData.ResetGoal();
				this->MoveData.DestinationActor = Actor;
				bRequestSuccessful = this->MoveData.MakeRequest(this->GetAIController());
			}
			break;
		case EAIMovementRequestType::TO_LOCATION:
			{
				auto Location = IMovementRequestInterface::Execute_GetLocation(Request);
				UE_VLOG(this, LogStateMachine, Verbose, TEXT("MovementRequest received for node %s (To Location: %s)"),
					*this->GetName(),
					*Location.ToString());
				this->StopMovement();
				this->MoveData.ResetGoal();
				this->MoveData.SetOverrideLocation(Location);
				bRequestSuccessful = this->MoveData.MakeRequest(this->GetAIController());
			}
			break;
		case EAIMovementRequestType::PAUSE:
			UE_VLOG(this, LogStateMachine, Verbose, TEXT("MovementRequest received for node %s (Pause Movement)"),
				*this->GetName());
			bRequestSuccessful = this->MoveData.PauseMove(this->GetAIController());
			break;
		case EAIMovementRequestType::RESUME:
			UE_VLOG(this, LogStateMachine, Verbose, TEXT("MovementRequest received for node %s (Resume Movement)"),
				*this->GetName());
			bRequestSuccessful = this->MoveData.ResumeMove(this->GetAIController());
			break;
	}

	if (!bRequestSuccessful)
	{
		this->EmitEvent(Events::AI::LOST);
	}
}

void UAISimpleMoveToNode::Enter_Inner_Implementation()
{
	this->MoveToPreset();
}

void UAISimpleMoveToNode::MoveToPreset()
{
	this->BindCallback();
	bool bRequestSuccessful = false;

	if (this->Property->IsBound())
	{
		bool bFoundData = false;
		auto& Value = this->Property->GetStruct<FMoveToData>(bFoundData);

		if (bFoundData)
		{
			bRequestSuccessful = Value.MakeRequest(this->GetAIController());
		}
	}
	else if (this->MoveData.bUseLocationIfNoGoal)
	{
		bRequestSuccessful = this->MoveData.MakeRequest(this->GetAIController());
	}

	if (!bRequestSuccessful)
	{
		this->EmitEvent(Events::AI::LOST);
	}
}

void UAISimpleMoveToNode::SetActive_Inner_Implementation(bool bNewActive)
{
	if (bNewActive)
	{
		this->BindCallback();
		this->MoveData.ResumeMove(this->GetAIController());
	}
	else
	{
		this->UnbindCallback();
		this->MoveData.PauseMove(this->GetAIController());
	}
}

void UAISimpleMoveToNode::EventWithData_Inner_Implementation(FName InEvent, UObject* Data, UObject* Source)
{
	/*
	this->UnbindCallback();

	if (auto Actor = UStateMachinePipedData::FindDataOfType<AActor>(Data))
	{		
		this->EnterWithData_Inner(Actor);
	}
	else if (auto Request = UStateMachinePipedData::FindDataImplementing<UMovementRequestInterface>(Data).GetObject())
	{
		switch (IMovementRequestInterface::Execute_GetRequestType(Request))
		{
			case EAIMovementRequestType::TO_ACTOR: StopMovement(); break;
			case EAIMovementRequestType::TO_LOCATION: StopMovement(); break;
		}
		this->EnterWithData_Inner(Request);
	}
	*/
}

void UAISimpleMoveToNode::StopMovement()
{
	this->GetAIController()->StopMovement();
}

void UAISimpleMoveToNode::OnMoveCompleted_Implementation(FAIRequestID RequestID, EPathFollowingResult::Type MoveResult)
{
	UE_VLOG(this, LogStateMachine, Verbose, TEXT("MoveCompleted"));
	this->MoveData.ResetGoal();
	this->MovementResult = MoveResult;

	switch (MoveResult)
	{
		case EPathFollowingResult::Success: this->EmitEvent(Events::AI::ARRIVE); break;
		case EPathFollowingResult::Aborted: this->EmitEvent(Events::AI::ARRIVE); break;
		default: this->EmitEvent(Events::AI::LOST);
	}
}

void UAISimpleMoveToNode::BindCallback()
{
	auto CtrlQ = this->GetAIController();
	CtrlQ->ReceiveMoveCompleted.AddUniqueDynamic(this, &UAISimpleMoveToNode::OnMoveCompleted);
}

void UAISimpleMoveToNode::UnbindCallback()
{
	auto CtrlQ = this->GetAIController();
	CtrlQ->ReceiveMoveCompleted.RemoveAll(this);
}

void UAISimpleMoveToNode::EventNotify_PauseMovement(FName InEvent, UObject* EventSource)
{
	this->MoveData.PauseMove(this->GetAIController());
}

void UAISimpleMoveToNode::EventNotify_ResumeMovement(FName InEvent, UObject* EventSource)
{
	this->MoveData.ResumeMove(this->GetAIController());
}