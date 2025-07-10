#include "StateMachine/Input/MultiInputNode.h"
#include "Engine/InputDelegateBinding.h"


void UMultiInputNode::Initialize_Inner_Implementation() {
	Super::Initialize_Inner_Implementation();

	this->PawnOwner = Cast<APawn>(this->GetMachine()->GetActorOwner());
}

void UMultiInputNode::BindInput()
{
	if (this->PawnOwner && this->PawnOwner->InputComponent)
	{
		if (!this->bIsBound)
		{
			this->bIsBound = true;
			UInputDelegateBinding::BindInputDelegates(this->GetClass(), this->PawnOwner->InputComponent, this);
		}
	}
}

void UMultiInputNode::UnbindInput()
{
	if (this->PawnOwner && this->PawnOwner->InputComponent)
	{
		if (this->bIsBound)
		{
			this->bIsBound = false;
			this->PawnOwner->InputComponent->ClearBindingsForObject(this);
		}
	}
}

void UMultiInputNode::SetActive_Inner_Implementation(bool bNewActive)
{
	if (bNewActive)
	{
		this->BindInput();
	}
	else
	{
		this->UnbindInput();
	}
}