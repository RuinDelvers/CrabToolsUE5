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

void UArrayNode::Event_Inner_Implementation(FName Event, UObject* EventSource)
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->Event(Event, EventSource);
		}
	}
}

void UArrayNode::EventWithData_Inner_Implementation(FName Event, UObject* Data, UObject* Source)
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->EventWithData(Event, Data, Source);
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

bool UArrayNode::UsesEnteringEvents_Implementation() const
{
	for (const auto& Child : this->Nodes)
	{
		if (Child && Child->UsesEnteringEvents())
		{
			return true;
		}
	}

	return false;
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

void UArrayNode::PostTransition_Implementation()
{
	for (auto& Child : this->Nodes)
	{
		if (Child)
		{
			Child->PostTransition();
		}
	}
}

UObject* UArrayNode::GetPipedData_Implementation()
{
	TArray<TObjectPtr<UObject>> DataArray;

	for (auto& Child : this->Nodes)
	{
		if (Child->HasPipedData())
		{
			DataArray.Add(Child->GetPipedData());
		}
	}
	
	return UStateMachinePipedData::ConcatDataArray(DataArray);
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