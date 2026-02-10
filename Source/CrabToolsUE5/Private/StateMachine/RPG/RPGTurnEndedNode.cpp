#include "StateMachine/RPG/RPGTurnEndedNode.h"
#include "Components/RPGSystem/RPGComponent.h"
#include "StateMachine/Events.h"

URPGTurnEndedNode::URPGTurnEndedNode()
{
	this->AddEmittedEvent(Events::RPG::TURN_ENDED);
}

void URPGTurnEndedNode::Enter_Inner_Implementation()
{
	this->Component->OnTurnEnd.AddDynamic(this, &URPGTurnEndedNode::TurnEnded);
}

void URPGTurnEndedNode::Exit_Inner_Implementation()
{
	this->Component->OnTurnEnd.RemoveAll(this);
}

void URPGTurnEndedNode::TurnEnded(URPGComponent* Comp)
{
	this->EmitEvent(Events::RPG::TURN_ENDED);
}
