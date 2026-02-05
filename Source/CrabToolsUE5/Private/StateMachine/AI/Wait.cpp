#include "StateMachine/AI/Wait.h"
#include "StateMachine/Events.h"

UAIWaitNode::UAIWaitNode()
{
	this->AddEmittedEvent(Events::AI::WAIT_FINISHED);
}

void UAIWaitNode::Enter_Inner_Implementation()
{
	this->GetWorld()->GetTimerManager().SetTimer(
		this->Callback, 
		this, 
		&UAIWaitNode::OnWaitEnd, 
		this->WaitTime,
		false);
}

void UAIWaitNode::Exit_Inner_Implementation()
{
	this->GetWorld()->GetTimerManager().ClearTimer(this->Callback);
}

void UAIWaitNode::SetActive_Inner_Implementation(bool bNewActive)
{
	if (bNewActive)
	{
		this->GetWorld()->GetTimerManager().UnPauseTimer(this->Callback);
	}
	else
	{
		this->GetWorld()->GetTimerManager().PauseTimer(this->Callback);
	}
		
}

void UAIWaitNode::OnWaitEnd()
{
	this->EmitEvent(Events::AI::WAIT_FINISHED);
}