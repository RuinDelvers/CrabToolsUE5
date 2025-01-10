#include "StateMachine/General/CallDelegateNode.h"
#include "Utils/UtilsLibrary.h"

UCallDelegateNode::UCallDelegateNode()
{
	this->EnterProperty = CreateDefaultSubobject<UStateMachineProperty>(TEXT("OnEnterCallback"));
	this->ExitProperty = CreateDefaultSubobject<UStateMachineProperty>(TEXT("OnExitCallback"));
	this->EnterProperty->Params = FSMPropertySearch::InlineDelegate(
		this->FindFunction(GET_FUNCTION_NAME_CHECKED(UCallDelegateNode, DelegateSignatureFunction)));

	this->ExitProperty->Params = FSMPropertySearch::InlineDelegate(
		this->FindFunction(GET_FUNCTION_NAME_CHECKED(UCallDelegateNode, DelegateSignatureFunction)));
}

void UCallDelegateNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();
}

void UCallDelegateNode::Exit_Inner_Implementation()
{
	if (auto Del = this->ExitProperty->GetProperty().GetValue<FMulticastScriptDelegate>())
	{
		Del->ProcessMulticastDelegate<UObject>(nullptr);
	}
}


void UCallDelegateNode::Enter_Inner_Implementation()
{
	if (auto Del = this->EnterProperty->GetProperty().GetValue<FMulticastScriptDelegate>())
	{
		Del->ProcessMulticastDelegate<UObject>(nullptr);
	}
}