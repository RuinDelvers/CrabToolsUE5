#include "StateMachine/General/SequentialNode.h"

void USequentialNode::Initialize_Inner_Implementation()
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->Initialize(this->GetMachine());
		}
	}
}

void USequentialNode::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->CurrentNode)
	{
		this->CurrentNode->Tick(DeltaTime);
	}
}

void USequentialNode::Enter_Inner_Implementation()
{
	this->Increment();
	if (this->CurrentNode)
	{
		this->CurrentNode->Enter();
	}
}

void USequentialNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	this->Increment();
	if (this->CurrentNode)
	{
		this->CurrentNode->EnterWithData(Data);
	}
}

void USequentialNode::Event_Inner_Implementation(FName InEvent)
{
	if (this->CurrentNode)
	{
		this->CurrentNode->Event(InEvent);
	}
}

void USequentialNode::EventWithData_Inner_Implementation(FName InEvent, UObject* Data)
{
	if (this->CurrentNode)
	{
		this->CurrentNode->EventWithData(InEvent, Data);
	}
}

void USequentialNode::SetActive_Inner_Implementation(bool bNewActive)
{
	if (this->CurrentNode)
	{
		this->CurrentNode->SetActive(bNewActive);
	}
}

void USequentialNode::Exit_Inner_Implementation()
{
	if (this->CurrentNode)
	{
		this->CurrentNode->Exit();
	}
}

bool USequentialNode::RequiresTick_Implementation() const
{
	
	if (this->CurrentNode)
	{
		return this->CurrentNode->RequiresTick();
	}
	else
	{
		return false;
	}
}

bool USequentialNode::HasPipedData_Implementation() const
{

	if (this->CurrentNode)
	{
		return this->CurrentNode->HasPipedData();
	}
	else
	{
		return false;
	}
}

UObject* USequentialNode::GetPipedData_Implementation()
{

	if (this->CurrentNode)
	{
		return this->CurrentNode->GetPipedData();
	}
	else
	{
		return nullptr;
	}
}

void USequentialNode::Increment()
{
	if (this->CurrentNode)
	{
		this->CurrentNode->Exit();
	}

	if (this->bLoop)
	{
		this->Index = this->Index + 1 % this->Nodes.Num();
		this->CurrentNode = this->Nodes[this->Index];
	}
	else
	{
		this->Index = FMath::Min(this->Index + 1, this->Nodes.Num() - 1);
		this->CurrentNode = this->Nodes[this->Index];		
	}

	if (this->CurrentNode)
	{
		this->CurrentNode->Enter();
	}
}

#if WITH_EDITOR
void USequentialNode::GetNotifies(TSet<FName>& Events) const
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->GetNotifies(Events);
		}
	}
}

void USequentialNode::GetEmittedEvents(TSet<FName>& Events) const
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->GetEmittedEvents(Events);
		}
	}
}
#endif