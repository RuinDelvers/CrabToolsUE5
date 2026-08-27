#include "StateMachine/General/SetBooleanNode.h"

USetBooleanNode::USetBooleanNode()
{
	this->Property.Properties.bCanWrite = true;
}


void USetBooleanNode::Exit_Inner_Implementation()
{
	if (this->bApplyOnExit)
	{
		this->Property.Set(this, this->bOnExit);
	}
}


void USetBooleanNode::Enter_Inner_Implementation()
{
	if (this->bApplyOnEnter)
	{
		this->Property.Set(this, this->bOnEnter);
	}
}
