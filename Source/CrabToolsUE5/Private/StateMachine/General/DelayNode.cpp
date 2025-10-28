#include "StateMachine/General/DelayNode.h"

void UDelayNode::Enter_Inner_Implementation()
{
	if (this->GetChild())
	{
		this->GetWorld()->GetTimerManager().SetTimer(
			this->Timer,
			this,
			&UDelayNode::HandleEnter,
			this->DelayTime,
			false);
	}
}

void UDelayNode::Exit_Inner_Implementation()
{
	if (this->Timer.IsValid())
	{
		this->GetWorld()->GetTimerManager().ClearTimer(this->Timer);
	}
	else
	{
		this->GetChild()->Exit();
	}
	
}

void UDelayNode::HandleEnter()
{
	this->GetChild()->Enter();
}