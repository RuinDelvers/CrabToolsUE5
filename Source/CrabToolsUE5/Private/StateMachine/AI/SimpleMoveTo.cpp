#include "StateMachine/AI/SimpleMoveTo.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/Events.h"
#include "StateMachine/AI/AIStructs.h"


UAISimpleMoveToNode::UAISimpleMoveToNode(): MovementResult(EPathFollowingResult::Aborted)
{
	this->AddEmittedEvent(Events::AI::ARRIVE);
	this->AddEmittedEvent(Events::AI::LOST);

	this->Property = CreateDefaultSubobject<UStateMachineProperty>(TEXT("MoveToTarget"));
	this->Property->Params = FSMPropertySearch::StructProperty(FMoveToData::StaticStruct());
}

void UAISimpleMoveToNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	check(this->GetAIController());
}

void UAISimpleMoveToNode::Exit_Inner_Implementation()
{
	this->UnbindCallback();

	this->StopMovement();

	this->OverrideLocation = FVector::Zero();
	this->bUseOverrideLocation = false;
}

void UAISimpleMoveToNode::PostTransition_Inner_Implementation()
{
	this->BindCallback();

	if (this->GoalActor)
	{
		if (this->bUseOverrideLocation)
		{
			this->GetAIController()->MoveToLocation(this->OverrideLocation);
		}
		else
		{
			this->MoveTo();
		}
	}
	else
	{
		if (auto Value = this->GetMovementData())
		{
			auto Ctrl = this->GetAIController();

			if (this->bUseOverrideLocation)
			{
				Ctrl->MoveToLocation(this->OverrideLocation);
			}
			else
			{
				if (Value->DestinationActor)
				{
					Ctrl->MoveToActor(Value->DestinationActor, 0.0f);
				}
				else
				{
					Ctrl->MoveToLocation(Value->DestinationLocation, 0.0f);
				}
			}
		}
	}
}

void UAISimpleMoveToNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	this->GoalActor = Cast<AActor>(Data);	
}

void UAISimpleMoveToNode::MoveTo()
{
	if (this->GoalActor)
	{
		auto Ctrl = this->GetAIController();

		if (Ctrl->IsFollowingAPath())
		{
			Ctrl->StopMovement();
		}

		Ctrl->MoveToActor(this->GoalActor, 0.0f);
	}
	else
	{
		this->EmitEvent(Events::AI::LOST);
	}
}


void UAISimpleMoveToNode::SetOverrideLocation(FVector Location)
{
	this->OverrideLocation = Location;
	this->bUseOverrideLocation = true;
}

FMoveToData* UAISimpleMoveToNode::GetMovementData() const
{
	return this->Property->GetProperty().GetValue<FMoveToData>();
}

void UAISimpleMoveToNode::StopMovement()
{
	this->GetAIController()->StopMovement();
}

void UAISimpleMoveToNode::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	this->MovementResult = Result;

	switch (Result)
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

#if WITH_EDITOR
void UAISimpleMoveToNode::PostLinkerChange()
{
	Super::PostLinkerChange();
}
#endif