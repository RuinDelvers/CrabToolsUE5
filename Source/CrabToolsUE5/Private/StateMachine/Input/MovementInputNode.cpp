#include "StateMachine/Input/MovementInputNode.h"

UMovementInputNode::UMovementInputNode() {
	this->bTrigger = true;
}


void UMovementInputNode::Initialize_Inner_Implementation() {
	Super::Initialize_Inner_Implementation();

	// If the owning actor has a perspective manager, store it for simple use.
	auto Persp = this->GetMachine()->GetActorOwner()->FindComponentByClass<UPerspectiveManager>();
	this->Perspective = Persp;
}



void UMovementInputNode::TriggerCallback_Implementation(const FInputActionValue& Value) {
	this->ApplyMovement(Value.Get<FVector2D>());
}

void UMovementInputNode::ApplyMovement_Implementation(FVector2D InputAxis) {
	// If our owner isn't a pawn, then we ignore it.
	auto Pawn = this->GetPawn();
	if (Pawn == nullptr) { return; }

	FRotator Rotation;

	if (this->Perspective.IsValid()) {
		Rotation = Perspective->GetPerspective();
	}
	else {
		Rotation = Pawn->Controller->GetControlRotation();
	}
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	Pawn->AddMovementInput(ForwardDirection, InputAxis.Y);
	Pawn->AddMovementInput(RightDirection, InputAxis.X);
}
