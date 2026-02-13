#include "StateMachine/AI/BaseNode.h"
#include "AIController.h"

void UAIBaseNode::Initialize_Inner_Implementation()
{
	if (auto PawnQ = Cast<APawn>(this->GetMachine()->GetOwner()))
	{
		this->AICtrl = Cast<AAIController>(PawnQ->GetController());
		PawnQ->ReceiveControllerChangedDelegate.AddDynamic(this, &UAIBaseNode::OnControllerUpdated);
	}
	else if (auto Ctrl = Cast<AAIController>(this->GetOwner())) 
	{
		this->AICtrl = Ctrl;		
	}
}

void UAIBaseNode::OnControllerUpdated(APawn* Pawn, AController* OldController, AController* NewController)
{
	this->AICtrl = Cast<AAIController>(NewController);
}
