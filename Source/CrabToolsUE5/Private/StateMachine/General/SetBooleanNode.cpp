#include "StateMachine/General/SetBooleanNode.h"

USetBooleanNode::USetBooleanNode()
{
	this->Property = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("FlagVariable"));
}

void USetBooleanNode::Initialize_Inner_Implementation()
{
	this->Property->Initialize();
}

void USetBooleanNode::Exit_Inner_Implementation()
{
	if (this->bOnExit != this->bOnEnter)
	{
		this->Property->SetBool(this->bOnExit);
	}
}


void USetBooleanNode::Enter_Inner_Implementation()
{
	this->Property->SetBool(this->bOnEnter);
}
