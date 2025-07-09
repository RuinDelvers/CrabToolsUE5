#include "StateMachine/Input/MultiInputNode.h"
#include "Engine/InputDelegateBinding.h"


void UMultiInputNode::Initialize_Inner_Implementation() {
	Super::Initialize_Inner_Implementation();

	this->PawnOwner = Cast<APawn>(this->GetMachine()->GetActorOwner());
}

void UMultiInputNode::Enter_Inner_Implementation()
{
	if (this->PawnOwner && this->PawnOwner->InputComponent)
	{
		UInputDelegateBinding::BindInputDelegates(this->GetClass(), this->PawnOwner->InputComponent, this);
	}
}

void UMultiInputNode::Exit_Inner_Implementation()
{
	if (this->PawnOwner && this->PawnOwner->InputComponent)
	{
		this->PawnOwner->InputComponent->ClearBindingsForObject(this);
	}
}
