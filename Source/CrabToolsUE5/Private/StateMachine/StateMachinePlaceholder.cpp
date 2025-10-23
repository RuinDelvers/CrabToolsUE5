#include "StateMachine/StateMachinePlaceholder.h"

void UStateMachinePlaceholderNode::Initialize_Inner_Implementation()
{
	/*
	if (auto SubMachine = this->GetMachine()->GetSharedMachines().Find(this->SlotName))
	{
		this->SubMachine = SubMachine->Get();
	}
	*/
}

void UStateMachinePlaceholderNode::Tick_Inner_Implementation(float DeltaTime)
{ 

}

void UStateMachinePlaceholderNode::Event_Inner_Implementation(FName Event) 
{ 

}
void UStateMachinePlaceholderNode::EventWithData_Inner_Implementation(FName InEvent, UObject* Data)
{ 

}

void UStateMachinePlaceholderNode::Enter_Inner_Implementation()
{ 

}

void UStateMachinePlaceholderNode::EnterWithData_Inner_Implementation(UObject* Data)
{ 

}

void UStateMachinePlaceholderNode::Exit_Inner_Implementation()
{

}

void UStateMachinePlaceholderNode::ExitWithData_Inner_Implementation(UObject* Data)
{

}