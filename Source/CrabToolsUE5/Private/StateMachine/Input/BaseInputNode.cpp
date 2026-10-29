#include "StateMachine/Input/BaseInputNode.h"


void UBaseInputNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	this->PawnOwner = CastChecked<APawn>(this->GetMachine()->GetActorOwner());
	this->Component = CastChecked<UEnhancedInputComponent>(this->PawnOwner->InputComponent);
}