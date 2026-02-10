#include "StateMachine/RPG/RPGTurnStartedNode.h"
#include "Components/RPGSystem/RPGComponent.h"
#include "StateMachine/Events.h"

URPGTurnStartedNode::URPGTurnStartedNode()
{
	this->AddEmittedEvent(Events::RPG::TURN_STARTED);
}

void URPGTurnStartedNode::Enter_Inner_Implementation()
{
	this->Component->OnTurnStart.AddDynamic(this, &URPGTurnStartedNode::TurnStarted);
}

void URPGTurnStartedNode::Exit_Inner_Implementation()
{
	this->Component->OnTurnStart.RemoveAll(this);
}

void URPGTurnStartedNode::TurnStarted(URPGComponent* Comp)
{
	this->EmitEvent(Events::RPG::TURN_STARTED);
}
