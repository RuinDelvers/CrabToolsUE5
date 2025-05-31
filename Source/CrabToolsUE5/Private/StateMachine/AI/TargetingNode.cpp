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
	if (IsValid(Data) && Data->Implements<UTargetingControllerInterface>())
	{
		this->TargetingInterface = Data;

		FConfirmTargetsSingle t;

		t.BindDynamic(this, &UTargetingNode::OnConfirmed);

		ITargetingControllerInterface::Execute_AddListener(Data, t);
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

bool UTargetingNode::HasPipedData_Implementation() const
{
	return IsValid(this->TargetingInterface);
}

UObject* UTargetingNode::GetPipedData_Implementation()
{
	return this->TargetingInterface;
}