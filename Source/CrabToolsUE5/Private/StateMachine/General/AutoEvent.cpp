#include "StateMachine/General/AutoEvent.h"

void UAutoEventNode::Enter_Inner_Implementation()
{
	if (!this->EmittedEvent.IsNone())
	{
		this->EmitEvent(this->EmittedEvent);
	}	
}

#if WITH_EDITOR
void UAutoEventNode::GetEmittedEvents(TSet<FName>& Events) const
{ 
	Events.Add(this->EmittedEvent); 
}
#endif //WITH_EDITOR