#include "StateMachine/Emitters/EventEmitter.h"
#include "StateMachine/StateMachine.h"

void UEventEmitter::Initialize(UStateMachine* POwner)
{
	this->Machine = POwner;
	this->Initialize_Inner();
}

void UEventEmitter::EmitEvent(FName Event)
{
	if (this->Machine)
	{
		this->Machine->SendEvent(Event);
	}
}

UStateMachine* UEventEmitter::GetMachine() const {
	return this->Machine;
}

AActor* UEventEmitter::GetOwner() const {
	return this->Machine->GetActorOwner();
}