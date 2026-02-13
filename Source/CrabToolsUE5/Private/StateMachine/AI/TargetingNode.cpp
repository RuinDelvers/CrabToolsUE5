#include "StateMachine/AI/TargetingNode.h"
#include "StateMachine/Events.h"
#include "Ability/TargetingController.h"


UTargetingNode::UTargetingNode()
{
	this->AddEmittedEvent(Events::AI::INVALID_TARGETING);
	this->AddEmittedEvent(Events::AI::TARGETS_CONFIRMED);
}

void UTargetingNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Targeting = UStateMachinePipedData::FindDataImplementing<UTargetingControllerInterface>(Data).GetObject())
	{
		this->TargetingInterface = Targeting;
	}
	else if (auto Spawner = UStateMachinePipedData::FindDataImplementing<USpawnsTargetingInterface>(Data).GetObject())
	{
		auto Interface = ISpawnsTargetingInterface::Execute_SpawnTargetingController(Spawner);
		this->TargetingInterface = Interface.GetObject();
	}

	if (this->TargetingInterface)
	{
		FConfirmTargetsSingle t;

		t.BindDynamic(this, &UTargetingNode::OnConfirmed);

		ITargetingControllerInterface::Execute_AddListener(this->TargetingInterface, t);
	}
	else
	{
		this->EmitEvent(Events::AI::INVALID_TARGETING);
	}
}

void UTargetingNode::OnConfirmed(TScriptInterface<ITargetingControllerInterface> Targeter)
{
	this->EmitEvent(Events::AI::TARGETS_CONFIRMED);
}

void UTargetingNode::Exit_Inner_Implementation()
{
	if (IsValid(this->TargetingInterface))
	{
		ITargetingControllerInterface::Execute_SetEnabled(this->TargetingInterface, false);
	}
}

bool UTargetingNode::HasPipedData_Implementation() const
{
	return IsValid(this->TargetingInterface);
}

UObject* UTargetingNode::GetPipedData_Implementation()
{
	return this->TargetingInterface;
}