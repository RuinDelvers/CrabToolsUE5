#include "StateMachine/General/RandomChoiceNode.h"

void URandomChoiceNode::Initialize_Inner_Implementation()
{
	for (const auto& Node : this->Nodes)
	{
		if (Node.Node)
		{
			Node.Node->Initialize(this->GetMachine());
		}
	}
}

void URandomChoiceNode::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->CurrentNode)
	{
		this->CurrentNode->Tick(DeltaTime);
	}
}

void URandomChoiceNode::Enter_Inner_Implementation()
{
	this->Increment();
	if (this->CurrentNode)
	{
		this->CurrentNode->Enter();
	}
}

void URandomChoiceNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	this->Increment();
	if (this->CurrentNode)
	{
		this->CurrentNode->EnterWithData(Data);
	}
}

void URandomChoiceNode::Event_Inner_Implementation(FName InEvent)
{
	if (this->CurrentNode)
	{
		this->CurrentNode->Event(InEvent);
	}
}

void URandomChoiceNode::EventWithData_Inner_Implementation(FName InEvent, UObject* Data)
{
	if (this->CurrentNode)
	{
		this->CurrentNode->EventWithData(InEvent, Data);
	}
}

void URandomChoiceNode::SetActive_Inner_Implementation(bool bNewActive)
{
	if (this->CurrentNode)
	{
		this->CurrentNode->SetActive(bNewActive);
	}
}

void URandomChoiceNode::Exit_Inner_Implementation()
{
	if (this->CurrentNode)
	{
		this->CurrentNode->Exit();
	}
}

bool URandomChoiceNode::RequiresTick_Implementation() const
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

bool URandomChoiceNode::HasPipedData_Implementation() const
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

UObject* URandomChoiceNode::GetPipedData_Implementation()
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

void URandomChoiceNode::Increment()
{
	if (this->Nodes.Num() == 0) { return; }

	auto& Data = this->Nodes[FMath::RandRange(0, this->Nodes.Num())];

	if (Index >= 0)
	{
		this->CurrentLife += 1;

		if (this->CurrentLife >= Data.LifeTime)
		{
			if (this->CurrentNode)
			{
				this->CurrentNode->SetActive(false);
			}

			this->CurrentNode = Data.Node;
			this->CurrentLife = 1;

			if (this->CurrentNode)
			{
				this->CurrentNode->SetActive(true);
			}
		}
		else
		{
			this->CurrentLife += 1;
		}
	}
	else
	{		
		this->CurrentNode = Data.Node;
		this->CurrentLife = 1;

		if (this->CurrentNode)
		{
			this->CurrentNode->SetActive(true);
		}
	}
}

#if WITH_EDITOR
void URandomChoiceNode::GetNotifies(TSet<FName>& Events) const
{
	for (const auto& Node : this->Nodes)
	{
		if (Node.Node)
		{
			Node.Node->GetNotifies(Events);
		}
	}
}

void URandomChoiceNode::GetEmittedEvents(TSet<FName>& Events) const
{
	for (const auto& Node : this->Nodes)
	{
		if (Node.Node)
		{
			Node.Node->GetEmittedEvents(Events);
		}
	}
}
#endif