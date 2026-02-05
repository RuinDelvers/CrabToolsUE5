#include "StateMachine/ArrayNode.h"

void UArrayNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();
	for (auto& Node : this->Nodes)
	{
		if (IsValid(Node))
		{
			Node->Initialize(this->GetMachine());
		}
	}
}

void UArrayNode::Tick_Inner_Implementation(float DeltaTime)
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->Tick(DeltaTime);
		}
	}
}

void UArrayNode::Event_Inner_Implementation(FName Event)
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->Event(Event);
		}
	}
}

void UArrayNode::EventWithData_Inner_Implementation(FName Event, UObject* Data)
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->EventWithData(Event, Data);
		}
	}
}

void UArrayNode::Enter_Inner_Implementation()
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->Enter();
		}
	}
}

void UArrayNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Array entering: %s with state"), *Node->GetClass()->GetName());
			Node->EnterWithData(Data);
		}
	}
}

void UArrayNode::Exit_Inner_Implementation()
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->Exit();	
		}
	}
}

void UArrayNode::ExitWithData_Inner_Implementation(UObject* Data)
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->ExitWithData(Data);
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
	if (this->GetNodeState() == EStateNodeState::ENTERED)
	{
		for (const auto& Child : this->Nodes)
		{
			Child->SetActive(bNewActive);
		}
	}
}

bool UArrayNode::Verify_Inner(FNodeVerificationContext& Context) const
{
	for (const auto& Child : this->Nodes)
	{
		if (Child)
		{
			if (!Child->Verify(Context))
			{
				return false;
			}
		}
		else
		{
			static FString Msg = "Null node found in ArrayNode";
			Context.Error(Msg, this);
		}
	}

	return true;
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