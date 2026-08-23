#include "StateMachine/Actors/ToggleDoorNode.h"
#include "Logging/MessageLog.h"

void UToggleDoorNode::Initialize_Inner_Implementation()
{
	this->Actor = this->GetOwner<ADoorActor>();
}

void UToggleDoorNode::Enter_Inner_Implementation()
{
	this->Actor->ApplyAction(this->EnterAction);
}

void UToggleDoorNode::Exit_Inner_Implementation()
{
	this->Actor->ApplyAction(this->ExitAction);
}