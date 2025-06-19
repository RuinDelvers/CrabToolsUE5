#include "StateMachine/AI/BaseNode.h"
#include "AIController.h"

void UAIBaseNode::Initialize_Inner_Implementation() {
	auto PawnQ = Cast<APawn>(this->GetMachine()->GetActorOwner());

	if (PawnQ) {
		this->PawnRef = PawnQ;

		this->AICtrl = Cast<AAIController>(PawnQ->GetController());
	}
}