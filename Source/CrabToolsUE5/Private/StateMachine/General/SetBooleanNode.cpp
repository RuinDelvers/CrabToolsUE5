#include "StateMachine/General/SetBooleanNode.h"
#include "Utils/UtilsLibrary.h"

USetBooleanNode::USetBooleanNode()
{
	this->Property = CreateDefaultSubobject<UStateMachineProperty>(TEXT("FlagVariable"));
	this->Property->Params = FSMPropertySearch::Property(FBoolProperty::StaticClass());
}

void USetBooleanNode::Exit_Inner_Implementation()
{
	auto& Prop = this->Property->GetProperty();

	if (Prop.IsValid())
	{
		Prop.SetValue<bool>(this->bOnExit);
	}
	else
	{
		UE_LOG(LogStateMachine, Error, TEXT("Invalid property found for SetBooleanNode"));
	}	
}


void USetBooleanNode::Enter_Inner_Implementation()
{
	auto& Prop = this->Property->GetProperty();

	if (Prop.IsValid())
	{
		Prop.SetValue<bool>(this->bOnEnter);
	}
	else
	{
		UE_LOG(LogStateMachine, Error, TEXT("Invalid property found for SetBooleanNode"));
	}
}
