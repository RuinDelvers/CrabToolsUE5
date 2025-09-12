#include "StateMachine/AI/AbilityNode.h"
#include "StateMachine/Events.h"
#include "StateMachine/IStateMachineLike.h"
#include "Ability/Ability.h"

UAbilityNode::UAbilityNode()
{
	this->AddEmittedEvent(Events::AI::ABILITY_FINISHED);
}

void UAbilityNode::Initialize_Inner_Implementation()
{
	if (IsValid(this->DefaultAbility))
	{
		this->DefaultAbility->Initialize(this->GetActorOwner());
	}
}

void UAbilityNode::Enter_Inner_Implementation()
{
	if (IsValid(this->Selected))
	{
		this->Selected->OnAbilityFinished.AddDynamic(this, &UAbilityNode::HandleFinish);

		this->Selected->Start();
	}
}

void UAbilityNode::Exit_Inner_Implementation()
{
	if (this->Selected)
	{
		this->Selected->OnAbilityFinished.RemoveAll(this);
		this->Selected = nullptr;
	}
}

bool UAbilityNode::RequiresTick_Implementation() const
{
	if (this->Selected)
	{
		return this->Selected->RequiresTick();
	}
	
	return false;
}

void UAbilityNode::PostTransition_Inner_Implementation()
{
	if (!IsValid(this->Selected))
	{
		this->EmitEvent(Events::AI::ABILITY_FINISHED);
	}
	else if (this->bNeedsFinishing)
	{
		this->EmitEvent(Events::AI::ABILITY_FINISHED);
	}

	this->bNeedsFinishing = false;
}

void UAbilityNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Abi = UStateMachineDataHelpers::FindDataOfType<UAbility>(Data))
	{
		this->Selected = Abi;
	}
	else
	{
		auto Value = UStateMachineDataHelpers::FindDataImplementing<UHasAbilityInterface>(Data);

		// Need to get object ref to check validity. if (Value) will always be false.
		if (Value.GetObjectRef())
		{
			this->Selected = IHasAbilityInterface::Execute_GetAbility(Value.GetObjectRef());
		}
		else
		{
			this->Selected = this->DefaultAbility;
		}		
	}

	this->Enter_Inner();
}

void UAbilityNode::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->Selected)
	{
		this->Selected->Tick(DeltaTime);
	}
}

void UAbilityNode::HandleFinish(UAbility* Abi)
{
	// Some abilities may finish just as they start, and if that happens, we'll need to delay handling
	// the end.
	if (this->GetMachine()->IsTransitioning())
	{
		this->bNeedsFinishing = true;
	}
	else
	{
		this->EmitEvent(Events::AI::ABILITY_FINISHED);
	}
}