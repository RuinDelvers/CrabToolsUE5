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
		this->DefaultAbility->Initialize(this->GetActorOwner(), nullptr);
	}
}

void UAbilityNode::Enter_Inner_Implementation()
{
	this->StartAbility(Selected, nullptr);
}

void UAbilityNode::Exit_Inner_Implementation()
{
	if (this->Selected)
	{
		this->Selected->OnAbilityFinished.RemoveAll(this);
		this->Selected = nullptr;
	}

	this->CachedPipedData = nullptr;
}

bool UAbilityNode::RequiresTick_Implementation() const
{
	if (this->Selected)
	{
		return this->Selected->RequiresTick();
	}
	
	return false;
}

bool UAbilityNode::HasPipedData_Implementation() const
{
	switch (this->PipedDateProcedure)
	{
		case EStateMachineGenericPipedDataProcedure::NONE: return false;
		case EStateMachineGenericPipedDataProcedure::DERIVED: return IsValid(this->Selected);
		case EStateMachineGenericPipedDataProcedure::CACHED: return IsValid(this->CachedPipedData);
		default: return false;
	}
}

UObject* UAbilityNode::GetPipedData_Implementation()
{
	switch (this->PipedDateProcedure)
	{
		case EStateMachineGenericPipedDataProcedure::NONE: return nullptr;
		case EStateMachineGenericPipedDataProcedure::DERIVED: return this->Selected;
		case EStateMachineGenericPipedDataProcedure::CACHED: return this->CachedPipedData;
		default: return nullptr;
	}
}

void UAbilityNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (this->PipedDateProcedure == EStateMachineGenericPipedDataProcedure::CACHED)
	{
		this->CachedPipedData = Data;
	}

	if (auto Abi = UCompositeObjectData::FindDataOfType<UAbility>(Data))
	{
		this->Selected = Abi;
	}
	else
	{
		auto Value = UCompositeObjectData::FindDataImplementing<UHasAbilityInterface>(Data);

		// Need to get object ref to check validity. if (Value) will always be false.
		if (IsValid(Value.GetObject()))
		{
			this->Selected = IHasAbilityInterface::Execute_GetAbility(Value.GetObjectRef());
		}
		else
		{
			this->Selected = this->DefaultAbility;
		}		
	}

	this->StartAbility(this->Selected, Data);
}

void UAbilityNode::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->Selected)
	{
		this->Selected->Tick(DeltaTime);
	}
}

void UAbilityNode::StartAbility(UAbility* Abi, UObject* StartData)
{
	if (IsValid(Abi))
	{
		Abi->OnAbilityFinished.AddDynamic(this, &UAbilityNode::HandleFinish);
		Abi->Start(StartData);
	}
	else
	{
		if (this->bNeedsFinishing)
		{
			this->EmitEvent(Events::AI::ABILITY_FINISHED);
		}
		else
		{
			this->EmitEvent(Events::AI::ABILITY_FINISHED);
		}
	}

	this->bNeedsFinishing = false;
}

void UAbilityNode::HandleFinish(UAbility* Abi)
{
	this->EmitEvent(Events::AI::ABILITY_FINISHED);
}