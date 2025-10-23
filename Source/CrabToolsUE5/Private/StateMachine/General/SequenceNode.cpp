#include "StateMachine/General/SequenceNode.h"

void USequenceNode::Initialize_Inner_Implementation()
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->Initialize(this->GetMachine());
		}
	}
}

void USequenceNode::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->CurrentNode)
	{
		this->CurrentNode->Tick(DeltaTime);
	}
}

void USequenceNode::Enter_Inner_Implementation()
{
	this->Increment();
	if (this->CurrentNode)
	{
		this->CurrentNode->Enter();
	}
}

void USequenceNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	this->Increment();
	if (this->CurrentNode)
	{
		this->CurrentNode->EnterWithData(Data);
	}
}

void USequenceNode::Event_Inner_Implementation(FName InEvent)
{
	if (this->CurrentNode)
	{
		this->CurrentNode->Event(InEvent);
	}
}

void USequenceNode::EventWithData_Inner_Implementation(FName InEvent, UObject* Data)
{
	if (this->CurrentNode)
	{
		this->CurrentNode->EventWithData(InEvent, Data);
	}
}

void USequenceNode::SetActive_Inner_Implementation(bool bNewActive)
{
	if (this->CurrentNode)
	{
		this->CurrentNode->SetActive(bNewActive);
	}
}

void USequenceNode::Exit_Inner_Implementation()
{
	if (this->CurrentNode)
	{
		this->CurrentNode->Exit();
	}
}

bool USequenceNode::RequiresTick_Implementation() const
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

bool USequenceNode::HasPipedData_Implementation() const
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

UObject* USequenceNode::GetPipedData_Implementation()
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

void USequenceNode::Increment()
{
	if (this->CurrentNode)
	{
		this->CurrentNode->SetActive(false);
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
		this->CurrentNode->SetActive(true);
	}
}

#if WITH_EDITOR
void USequenceNode::GetNotifies(TSet<FName>& Events) const
{
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->GetNotifies(Events);
		}
	}
}

void USequenceNode::GetEmittedEvents(TSet<FName>& Events) const
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