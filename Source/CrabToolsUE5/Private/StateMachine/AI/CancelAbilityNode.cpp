#include "StateMachine/AI/CancelAbilityNode.h"
#include "StateMachine/Events.h"
#include "Ability/Ability.h"

UCancelAbilityNode::UCancelAbilityNode()
{
	this->AddEmittedEvent(Events::AI::DONE);
}


void UCancelAbilityNode::PostTransition_Inner_Implementation()
{
	this->EmitEvent(Events::AI::DONE);
}

void UCancelAbilityNode::Enter_Inner_Implementation()
{
	if (this->bCanCallOwner && this->GetOwner()->Implements<UHasAbilityInterface>())
	{
		if (auto GetAbi = IHasAbilityInterface::Execute_GetAbility(this->GetOwner()))
		{
			GetAbi->Cancel();
		}
	}
}

void UCancelAbilityNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	UE_LOG(LogTemp, Warning, TEXT("This path was taken?"));
	if (auto Abi = Cast<UAbility>(Data))
	{
		Abi->Cancel();
	}
	else if (Data->Implements<UHasAbilityInterface>())
	{
		if (auto GetAbi = IHasAbilityInterface::Execute_GetAbility(Data))
		{
			GetAbi->Cancel();
		}
		else if (this->bCanCallOwner && this->GetOwner()->Implements<UHasAbilityInterface>())
		{
			if (auto GetAbi2 = IHasAbilityInterface::Execute_GetAbility(this->GetOwner()))
			{
				GetAbi2->Cancel();
			}
		}
	}
	else if (this->bCanCallOwner && this->GetOwner()->Implements<UHasAbilityInterface>())
	{
		if (auto GetAbi = IHasAbilityInterface::Execute_GetAbility(this->GetOwner()))
		{
			GetAbi->Cancel();
		}
	}
}
