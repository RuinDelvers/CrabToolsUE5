#include "StateMachine/AI/CancelAbilityNode.h"
#include "StateMachine/Events.h"
#include "Ability/Ability.h"

UCancelAbilityNode::UCancelAbilityNode()
{
	this->AddEmittedEvent(Events::AI::DONE);
}

void UCancelAbilityNode::Enter_Inner_Implementation()
{
	this->HandleOwnerCall();
	this->EmitEvent(Events::AI::DONE);
}

void UCancelAbilityNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Abi = UStateMachinePipedData::FindDataOfType<UAbility>(Data))
	{
		Abi->Cancel();
	}
	else if (auto Value = UStateMachinePipedData::FindDataImplementing<UHasAbilityInterface>(Data).GetObject())
	{
		// Need to get object ref to check validity. if (Value) will always be false.
		if (auto GetAbi = IHasAbilityInterface::Execute_GetAbility(Value))
		{
			GetAbi->Cancel();
		}
	}

	this->EmitEvent(Events::AI::DONE);
}

void UCancelAbilityNode::HandleOwnerCall()
{
	if (this->bCanCallOwner)
	{
		if (auto Abi = Cast<UAbility>(this->GetOwner()))
		{
			Abi->Cancel();
		}
		if (this->GetOwner()->Implements<UHasAbilityInterface>())
		{
			if (auto GetAbi = IHasAbilityInterface::Execute_GetAbility(this->GetOwner()))
			{
				GetAbi->Cancel();
			}
		}
	}
}
