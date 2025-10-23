#include "StateMachine/AI/RotateToNode.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/Events.h"

UAIRotateToNode::UAIRotateToNode()
{
	this->AddEmittedEvent(Events::AI::ARRIVE);
	this->AddEmittedEvent(Events::AI::LOST);

	this->Property = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("RotationTarget"));
}

void UAIRotateToNode::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->TargetActor.IsValid())
	{
		if (this->FacingCheck())
		{
			this->EmitEvent(Events::AI::ARRIVE);
		}
	}
	else
	{
		this->EmitEvent(Events::AI::LOST);
	}
}

void UAIRotateToNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();
}

void UAIRotateToNode::Exit_Inner_Implementation()
{
	this->TargetActor = nullptr;
}

void UAIRotateToNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Actor = Cast<AActor>(Data))
	{
		this->TargetActor = Actor;

		if (this->TargetActor.IsValid())
		{
			if (auto Ctrl = this->GetAIController())
			{
				Ctrl->SetFocus(this->TargetActor.Get());
			}
			else
			{
				this->TargetActor = nullptr;
			}
		}
	}

	if (!this->TargetActor.IsValid())
	{
		this->EmitEvent(Events::AI::LOST);
	}
}

void UAIRotateToNode::Enter_Inner_Implementation()
{
	if (auto Value = this->Property->GetObject<AActor>())
	{	
		this->TargetActor = Value;

		if (auto Ctrl = this->GetAIController())
		{
			Ctrl->SetFocus(this->TargetActor.Get());
		}
		else
		{
			this->TargetActor = nullptr;
		}
	}

	if (!this->TargetActor.IsValid())
	{
		this->EmitEvent(Events::AI::LOST);
	}
}

bool UAIRotateToNode::FacingCheck() const
{
	// Whether or not we are currently facing the focus point.
	bool Check = false;

	if (auto Ctrl = this->GetAIController())
	{
		if (this->TargetActor.IsValid())
		{
			FVector Facing = this->TargetActor.Get()->GetActorLocation() - this->GetActorOwner()->GetActorLocation();
			FVector ToTarget = Ctrl->GetFocalPoint() - this->GetActorOwner()->GetActorLocation();

			if (this->bIgnorePitch)
			{
				// Project the vectors onto the ground.
				Facing.Z = 0;
				ToTarget.Z = 0;				
			}

			Facing.Normalize();
			ToTarget.Normalize();

			Check = FMath::Abs(Facing.Dot(ToTarget) - 1) < 1e-3f;
		}
	}
	else
	{
		Check = true;
	}

	return Check;
}

#if WITH_EDITOR
void UAIRotateToNode::PostLinkerChange()
{
	Super::PostLinkerChange();
}
#endif // WITH_EDITOR