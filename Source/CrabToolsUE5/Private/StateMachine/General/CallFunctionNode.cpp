#include "StateMachine/General/CallFunctionNode.h"


void UCallFunctionNode::Enter_Inner_Implementation()
{
	for (auto& Data : this->Functions)
	{
		if (Data.bCallOnEnter)
		{
			Data.Function.Execute(this);
		}
	}
	
}

void UCallFunctionNode::Exit_Inner_Implementation()
{
	for (auto& Data : this->Functions)
	{
		if (Data.bCallOnExit)
		{
			Data.Function.Execute(this);
		}
	}
}