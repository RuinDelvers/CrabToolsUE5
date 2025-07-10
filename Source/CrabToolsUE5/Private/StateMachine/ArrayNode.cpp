#include "StateMachine/ArrayNode.h"

void UArrayNode::Initialize_Inner_Implementation() {
	Super::Initialize_Inner_Implementation();
	for (auto& Node : this->Nodes) {
		if (IsValid(Node))
		{
			Node->Initialize(this->GetMachine());
		}
	}
}

void UArrayNode::Tick_Inner_Implementation(float DeltaTime) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->Tick(DeltaTime);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::Event_Inner_Implementation(FName Event) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->Event(Event);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::EventWithData_Inner_Implementation(FName Event, UObject* Data) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->EventWithData(Event, Data);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::Enter_Inner_Implementation() {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->Enter();
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::EnterWithData_Inner_Implementation(UObject* Data) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->EnterWithData(Data);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::Exit_Inner_Implementation() {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->Exit();	
			if (!(!this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::ExitWithData_Inner_Implementation(UObject* Data) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->ExitWithData(Data);
			if (!(!this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::PostTransition_Inner_Implementation()
{
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->PostTransition();
			if (!(!this->Active() && this->GetMachine()->IsInState(TID)))
			{
				return;
			}
		}
	}
}

bool UArrayNode::RequiresTick_Implementation() const
{
	for (const auto& Node : this->Nodes)
	{
		if (IsValid(Node) && Node->RequiresTick())
		{
			return true;
		}
	}

	return false;
}

bool UArrayNode::HasPipedData_Implementation() const
{
	for (const auto& Child : this->Nodes)
	{
		if (Child->HasPipedData())
		{
			return true;
		}
	}

	return false;
}

void UArrayNode::SetActive_Inner_Implementation(bool bNewActive)
{
	for (const auto& Child : this->Nodes)
	{
		Child->SetActive(bNewActive);
	}
}

UObject* UArrayNode::GetPipedData_Implementation()
{
	auto Data = NewObject<UArrayNodeData>(this);

	for (auto& Child : this->Nodes)
	{
		if (Child->HasPipedData())
		{
			Data->AddData(Child->GetPipedData());
		}
	}

	return Data;
}

void UArrayNodeData::AddData(UObject* AddedData)
{
	if (IsValid(AddedData))
	{
		this->Data.Add(AddedData);
	}
}

UObject* UArrayNodeData::FindDataOfType_Implementation(TSubclassOf<UObject> Type)
{
	for (auto& Child : this->Data)
	{
		if (auto Check = UStateMachineDataHelpers::FindDataOfType(Type, Child))
		{
			return Check;
		}
	}

	return nullptr;
}

void UArrayNodeData::FindAllDataOfType_Implementation(TSubclassOf<UObject> Type, TArray<UObject*>& ReturnValue)
{
	for (auto& Child : this->Data)
	{
		if (Child->IsA(Type))
		{
			ReturnValue.Add(Child);
		}
		
		UStateMachineDataHelpers::FindAllDataOfType(Type, Child, ReturnValue);
	}
}

TScriptInterface<UInterface> UArrayNodeData::FindDataImplementing_Implementation(TSubclassOf<UInterface> Type)
{
	for (auto& Child : this->Data)
	{
		if (Child->GetClass()->ImplementsInterface(Type))
		{
			TScriptInterface<UInterface> T(Child);

			return T;
		}
	}

	return nullptr;
}

void UArrayNodeData::FindAllDataImplementing_Implementation(TSubclassOf<UInterface> Type, TArray<TScriptInterface<UInterface>>& ReturnValue)
{
	for (auto& Child : this->Data)
	{
		if (Child->GetClass()->ImplementsInterface(Type))
		{
			TScriptInterface<UInterface> T(Child);

			ReturnValue.Add(T);
		}

		UStateMachineDataHelpers::FindAllDataImplementing(Type, Child, ReturnValue);
	}
}

#if WITH_EDITORONLY_DATA
void UArrayNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Super::GetEmittedEvents(Events);

	for (auto& Child : this->Nodes)
	{
		Child->GetEmittedEvents(Events);
	}
}
#endif