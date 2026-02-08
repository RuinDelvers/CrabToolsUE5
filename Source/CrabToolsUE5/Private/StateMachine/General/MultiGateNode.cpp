#include "StateMachine/General/MultiGateNode.h"
#include "StateMachine/ArrayNode.h"

void UMultiGateNode::Initialize_Inner_Implementation()
{
	for (auto& Pair : this->Nodes)
	{
		check(Pair.Node);
		check(Pair.Gate);
			
		Pair.Node->Initialize(this->GetMachine());

		Pair.Gate->Initialize(this->GetMachine());
		Pair.Gate->OnGateStateChanged.AddUFunction(
			this,
			GET_FUNCTION_NAME_CHECKED(UMultiGateNode, OnGateChanged),
			Pair.Node,
			Pair.Gate);
	}
}

bool UMultiGateNode::Verify_Inner(FNodeVerificationContext& Context) const
{
	for (const auto& Pair : this->Nodes)
	{
		if (!Pair.Node || !Pair.Gate)
		{
			static FString Msg = "MultiGateNode has Node or Gate as None.";
			Context.Error(Msg, this);

			return false;
		}
	}

	return true;
}

void UMultiGateNode::Tick_Inner_Implementation(float DeltaTime)
{
	for (const auto& Child : this->Nodes)
	{
		Child.Node->Tick(DeltaTime);
	}
}

void UMultiGateNode::Event_Inner_Implementation(FName Event)
{
	for (const auto& Child : this->Nodes)
	{
		Child.Node->Event(Event);
	}
}

void UMultiGateNode::EventWithData_Inner_Implementation(FName InEvent, UObject* Data)
{
	for (const auto& Child : this->Nodes)
	{
		Child.Node->EventWithData(InEvent, Data);
	}
}

void UMultiGateNode::Enter_Inner_Implementation()
{
	for (const auto& Child : this->Nodes)
	{
		Child.Node->Enter();
	}

	for (const auto& Data : this->Nodes)
	{
		this->OnGateChanged(Data.Node, Data.Gate);
	}
}

void UMultiGateNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	for (const auto& Child : this->Nodes)
	{
		Child.Node->EnterWithData(Data);
	}

	for (const auto& Pair : this->Nodes)
	{
		this->OnGateChanged(Pair.Node, Pair.Gate);
	}
}

void UMultiGateNode::Exit_Inner_Implementation()
{
	for (const auto& Child : this->Nodes)
	{
		Child.Node->Exit();
	}
}

void UMultiGateNode::ExitWithData_Inner_Implementation(UObject* Data)
{
	for (const auto& Child : this->Nodes)
	{
		Child.Node->ExitWithData(Data);
	}
}

bool UMultiGateNode::RequiresTick_Implementation() const
{
	for (const auto& Child : this->Nodes)
	{
		if (Child.Node->RequiresTick())
		{
			return true;
		}
	}

	return false;
}

bool UMultiGateNode::HasPipedData_Implementation() const
{
	for (const auto& Child : this->Nodes)
	{
		if (Child.Node->HasPipedData())
		{
			return true;
		}
	}

	return false;
}

UObject* UMultiGateNode::GetPipedData_Implementation()
{
	auto Data = NewObject<UArrayNodeData>(this);

	for (auto& Child : this->Nodes)
	{
		if (Child.Node->HasPipedData())
		{
			Data->AddData(Child.Node->GetPipedData());
		}
	}

	return Data;
}

void UMultiGateNode::SetActive_Inner_Implementation(bool bNewActive)
{
	for (const auto& Pair : this->Nodes)
	{
		Pair.Node->SetActive(Pair.Gate->GetGateActivity() && bNewActive);
	}
}

bool UMultiGateNode::DoesReferenceMachine_Inner_Implementation(FName MachineName) const
{
	for (const auto& Data : this->Nodes)
	{
		if (Data.Node && Data.Node->DoesReferenceMachine(MachineName))
		{
			return true;
		}
	}
	return false;
}

void UMultiGateNode::OnGateChanged(UStateNode* Node, UMultiGateNodeGate* Gate)
{
	bool bNewActivity = Gate->GetGateActivity() && this->Active();

	Node->SetActive(bNewActivity);
}

void UMultiGateNodeGate::UpdateGate() const
{
	this->OnGateStateChanged.Broadcast();
}

void UMultiGateNodeGate::Initialize(UStateMachine* InitMachine)
{
	this->Machine = InitMachine;
	this->Initialize_Inner();
}
