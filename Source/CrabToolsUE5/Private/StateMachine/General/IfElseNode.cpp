#include "StateMachine/General/IfElseNode.h"
#include "Properties/GenericPropertyBinding.h"

UIfElseNode::UIfElseNode()
{
	this->Property = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("Property"));
}

UStateNode* UIfElseNode::GetNode() const
{
	return this->GetFlag() ? this->TrueNode : this->FalseNode;
}

bool UIfElseNode::GetFlag() const
{
	switch (this->ConditionType)
	{
		case EIfElseNodeFlagType::FLAG: return this->bFlag;
		case EIfElseNodeFlagType::PROPERTY: return this->Property->GetBool();
		default: return true;
	}
}

void UIfElseNode::Initialize_Inner_Implementation()
{
	if (this->TrueNode) { this->TrueNode->Initialize(this->GetMachine()); }
	if (this->FalseNode) { this->FalseNode->Initialize(this->GetMachine()); }
}

void UIfElseNode::Tick_Inner_Implementation(float DeltaTime)
{
	if (auto Node = this->GetNode()) { Node->Tick(DeltaTime); }
}

void UIfElseNode::Event_Inner_Implementation(FName Event)
{
	if (auto Node = this->GetNode()) { Node->Event(Event); }
}

void UIfElseNode::EventWithData_Inner_Implementation(FName EName, UObject* Data)
{
	if (auto Node = this->GetNode()) { Node->EventWithData(EName, Data); }
}

void UIfElseNode::Enter_Inner_Implementation()
{
	if (auto Node = this->GetNode()) { Node->Enter(); }
}

void UIfElseNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Node = this->GetNode()) { Node->EnterWithData(Data); }
}

void UIfElseNode::Exit_Inner_Implementation()
{
	if (auto Node = this->GetNode()) { Node->Exit(); }
}

void UIfElseNode::ExitWithData_Inner_Implementation(UObject* Data)
{
	if (auto Node = this->GetNode()) { Node->ExitWithData(Data); }
}

bool UIfElseNode::RequiresTick_Implementation() const
{
	if (auto Node = this->GetNode())
	{
		return Node->RequiresTick();
	}
	else
	{
		return false;
	}
}

void UIfElseNode::PostTransition_Inner_Implementation()
{
	if (auto Node = this->GetNode()) { Node->PostTransition(); }
}

void UIfElseNode::SetActive_Inner_Implementation(bool bNewActive)
{
	if (auto Node = this->GetNode()) { Node->SetActive(bNewActive); }
}

bool UIfElseNode::HasPipedData_Implementation() const
{
	if (auto Node = this->GetNode())
	{
		return Node->HasPipedData();
	}
	else
	{
		return false;
	}
}

UObject* UIfElseNode::GetPipedData_Implementation()
{
	if (auto Node = this->GetNode())
	{
		return Node->GetPipedData();
	}
	else
	{
		return nullptr;
	}
}

#if WITH_EDITORONLY_DATA
void UIfElseNode::GetEmittedEvents(TSet<FName>& Events) const
{
	if (this->TrueNode)
	{
		this->TrueNode->GetEmittedEvents(Events);
	}

	if (this->FalseNode)
	{
		this->FalseNode->GetEmittedEvents(Events);
	}
}
#endif