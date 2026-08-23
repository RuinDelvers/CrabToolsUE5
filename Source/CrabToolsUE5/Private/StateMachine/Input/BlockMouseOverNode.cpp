#include "StateMachine/Input/BlockMouseOverNode.h"
#include "Components/MouseOverComponent.h"

void UBlockMouseOverNode::Initialize_Inner_Implementation()
{
	this->MouseOver = this->GetActorOwner()->GetComponentByClass<UMouseOverComponent>();
}

void UBlockMouseOverNode::Exit_Inner_Implementation()
{
	if (this->bInvert)
	{
		this->MouseOver->AddBlocker(this);
	}
	else
	{
		this->MouseOver->RemoveBlocker(this);
	}
}

void UBlockMouseOverNode::Enter_Inner_Implementation()
{
	if (this->bInvert)
	{
		this->MouseOver->RemoveBlocker(this);
	}
	else
	{
		this->MouseOver->AddBlocker(this);
	}
}
