#include "StateMachine/General/AutoEvent.h"

void UAutoEventNode::PostTransition_Inner_Implementation()
{
	if (!this->EmittedEvent.IsNone())
	{
		this->GetMachine()->SendEvent(this->EmittedEvent);
	}	
}

#if WITH_EDITOR
void UAutoEventNode::GetEmittedEvents(TSet<FName>& Events) const
{ 
	Events.Add(this->EmittedEvent); 
}
#endif //WITH_EDITOR