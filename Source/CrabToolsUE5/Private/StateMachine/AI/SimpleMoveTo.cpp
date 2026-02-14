#include "StateMachine/AI/SimpleMoveTo.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/Events.h"
#include "StateMachine/AI/AIStructs.h"
#include "AI/MovementRequest.h"


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
	this->MoveData.PauseMove(this->GetAIController());
}

void UAISimpleMoveToNode::Enter_Inner_Implementation()
{
	this->BindCallback();

	if (this->Property->IsBound())
	{
		bool bFoundData = false;
		auto& Value = this->Property->GetStruct<FMoveToData>(bFoundData);

		if (bFoundData)
		{
			Value.MakeRequest(this->GetAIController());

			if (this->MoveData.Result.Code == EPathFollowingRequestResult::Failed)
			{
				this->EmitEvent(Events::AI::LOST);
			}
		}
	}
	else
	{
		this->MoveData.MakeRequest(this->GetAIController());
	}
}

void UAISimpleMoveToNode::SetActive_Inner_Implementation(bool bNewActive)
{
	if (bNewActive)
	{
		this->MoveData.ResumeMove(this->GetAIController());
	}
	else
	{
		this->MoveData.PauseMove(this->GetAIController());
	}
}

void UAISimpleMoveToNode::EventWithData_Inner_Implementation(FName InEvent, UObject* Data, UObject* Source)
{
	if (auto Actor = UStateMachinePipedData::FindDataOfType<AActor>(Data))
	{
		this->UnbindCallback();
		this->StopMovement();
		this->EnterWithData_Inner(Actor);
	}
	else if (auto Request = UStateMachinePipedData::FindDataImplementing<UMovementRequestInterface>(Data).GetObject())
	{
		this->UnbindCallback();
		switch (IMovementRequestInterface::Execute_GetRequestType(Request))
		{
			case EAIMovementRequestType::TO_ACTOR: StopMovement(); break;
			case EAIMovementRequestType::TO_LOCATION: StopMovement(); break;
		}
		this->EnterWithData_Inner(Request);
	}	
}

void UAISimpleMoveToNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Actor = UStateMachinePipedData::FindDataOfType<AActor>(Data))
	{
		this->MoveData.DestinationActor = Actor;
		this->Enter_Inner();
	}
	else if (auto Request = UStateMachinePipedData::FindDataImplementing<UMovementRequestInterface>(Data).GetObject())
	{
		switch (IMovementRequestInterface::Execute_GetRequestType(Request))
		{
			case EAIMovementRequestType::TO_ACTOR: 
				this->MoveData.DestinationActor = IMovementRequestInterface::Execute_GetActor(Request);
				break;
			case EAIMovementRequestType::TO_LOCATION:
				this->SetOverrideLocation(IMovementRequestInterface::Execute_GetLocation(Request));
				break;
			case EAIMovementRequestType::PAUSE:
				if (!this->MoveData.PauseMove(this->GetAIController()))
				{
					this->EmitEvent(Events::AI::LOST);
				}
				break;
			case EAIMovementRequestType::RESUME:
				if (!this->MoveData.ResumeMove(this->GetAIController()))
				{
					this->EmitEvent(Events::AI::LOST);
				}
				break;
		}
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
		default: this->EmitEvent(Events::AI::LOST);
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

void UAISimpleMoveToNode::EventNotify_PauseMovement(FName InEvent, UObject* EventSource)
{
	this->MoveData.PauseMove(this->GetAIController());
}

void UAISimpleMoveToNode::EventNotify_ResumeMovement(FName InEvent, UObject* EventSource)
{
	this->MoveData.ResumeMove(this->GetAIController());
}