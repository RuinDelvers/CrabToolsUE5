#include "StateMachine/General/CallDelegateNode.h"
#include "Utils/UtilsLibrary.h"

UCallDelegateNode::UCallDelegateNode()
{
	this->EnterProperty = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("OnEnterCallback"));
	this->ExitProperty = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("OnExitCallback"));
}

void UCallDelegateNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();
}

void UCallDelegateNode::Exit_Inner_Implementation()
{
	this->ExitProperty->CallFunction();
}


void UCallDelegateNode::Enter_Inner_Implementation()
{
	this->ExitProperty->CallFunction();
}