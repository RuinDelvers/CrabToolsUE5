#include "StateMachine/General/CallDelegateNode.h"
#include "Utils/UtilsLibrary.h"

UCallDelegateNode::UCallDelegateNode()
{

}

void UCallDelegateNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();
}

void UCallDelegateNode::Exit_Inner_Implementation()
{
	this->ExitProperty.Execute(this);
}


void UCallDelegateNode::Enter_Inner_Implementation()
{
	this->EnterProperty.Execute(this);
}