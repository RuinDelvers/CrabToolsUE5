#include "StateMachine/AI/SimpleMoveTo.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/Events.h"
#include "StateMachine/AI/AIStructs.h"
#include "AI/MovementRequest.h"
#include "StateMachine/Logging.h"
#include "NavigationPath.h"


UAISimpleMoveToNode::UAISimpleMoveToNode()
{
	this->AddEmittedEvent(Events::AI::ARRIVE);
	this->AddEmittedEvent(Events::AI::ABORT);
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
	if (auto Request = UCompositeObjectData::FindDataImplementing<UMovementRequestInterface>(Data).GetObject())
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
				this->UnbindCallback();
				this->StopMovement();
				this->BindCallback();
				this->OnRequestStarted();
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
				this->UnbindCallback();
				this->StopMovement();
				this->BindCallback();
				this->OnRequestStarted();
				this->MoveData.ResetGoal();
				this->MoveData.SetOverrideLocation(Location);
				bRequestSuccessful = this->MoveData.MakeRequest(this->GetAIController());
			}
			break;
		case EAIMovementRequestType::PAUSE:
			UE_VLOG(this, LogStateMachine, Verbose, TEXT("MovementRequest received for node %s (Pause Movement)"),
				*this->GetName());
			this->BindCallback();
			this->OnRequestStarted();
			bRequestSuccessful = this->MoveData.PauseMove(this->GetAIController());
			break;
		case EAIMovementRequestType::RESUME:
			UE_VLOG(this, LogStateMachine, Verbose, TEXT("MovementRequest received for node %s (Resume Movement)"),
				*this->GetName());
			this->OnRequestStarted();
			this->BindCallback();
			bRequestSuccessful = this->MoveData.ResumeMove(this->GetAIController());
			break;
		case EAIMovementRequestType::PATH:
			{
				auto Path = IMovementRequestInterface::Execute_GetNavPath(Request);
				UE_VLOG(this, LogStateMachine, Verbose, TEXT("MovementRequest received for node %s (Along Path from %s to %s)"),
					*this->GetName(),
					*Path->GetPath()->GetStartLocation().ToString(),
					*Path->GetPath()->GetEndLocation().ToString());
				this->UnbindCallback();
				this->StopMovement();
				this->BindCallback();
				this->OnRequestStarted();
				this->MoveData.ResetGoal();
				bRequestSuccessful = this->MoveData.MakeRequest(this->GetAIController(), Path);
			}
			break;
		case EAIMovementRequestType::STOP:
			this->StopMovement();
			this->EmitEvent(Events::AI::ABORT);
			bRequestSuccessful = true;
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
	
	bool bRequestSuccessful = false;

	if (this->Property->IsBound())
	{
		bool bFoundData = false;
		auto& Value = this->Property->GetStruct<FMoveToData>(bFoundData);

		if (bFoundData)
		{
			this->BindCallback();
			this->OnRequestStarted();
			bRequestSuccessful = Value.MakeRequest(this->GetAIController());
		}
	}
	else if (this->MoveData.bUseLocationIfNoGoal)
	{
		this->BindCallback();
		this->OnRequestStarted();
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

	if (auto Request = UCompositeObjectData::FindDataImplementing<UMovementRequestInterface>(Data).GetObject())
	{
		this->MoveToRequest(Request);
	}
}

void UAISimpleMoveToNode::StopMovement()
{
	this->GetAIController()->StopMovement();
}

void UAISimpleMoveToNode::OnMoveCompleted_Implementation(FAIRequestID RequestID, EPathFollowingResult::Type MoveResult)
{
	UE_VLOG(this, LogStateMachine, Verbose, TEXT("MoveCompleted (%s)"), *UEnum::GetValueAsString(MoveResult));
	this->MoveData.ResetGoal();

	switch (MoveResult)
	{
		case EPathFollowingResult::Success: this->EmitEvent(Events::AI::ARRIVE); break;
		case EPathFollowingResult::Aborted: this->EmitEvent(Events::AI::ABORT); break;
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