#include "StateMachine/EdGraph/EdAliasNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"

#define LOCTEXT_NAMESPACE "EdAliasNode"

UEdAliasNode::UEdAliasNode() {
	this->bCanRenameNode = true;
}

UEdAliasNode::~UEdAliasNode() {

}

FName UEdAliasNode::GetStateName() const
{ 
	return this->AliasLabel;
}

FName UEdAliasNode::SetStateName(FName NewName)
{
	FName OldName = this->AliasLabel;
	this->AliasLabel = NewName;
	this->Events.OnNameChanged.Broadcast(OldName, NewName);

	return NewName;
}

void UEdAliasNode::RenameNode(FName Name)
{
	this->GetStateGraph()->RenameNode(this, Name);
}

bool UEdAliasNode::Matches(const UEdStateNode* Node) const
{
	return this->AliasedStates.Contains(Node->GetStateName()) != this->bComplement;
}

TArray<FString> UEdAliasNode::GetAvailableStates() const
{
	TArray<FString> Names;

	for (auto& State : this->GetStateGraph()->GetStates())
	{
		Names.Add(State->GetStateName().ToString());
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TSet<FName> UEdAliasNode::GetAliasedStates() const
{
	TSet<FName> Names;

	for (auto& State : this->GetStateGraph()->GetStates())
	{
		if (this->AliasedStates.Contains(State->GetStateName()) != this->bComplement)
		{
			Names.Add(State->GetStateName());
		}
	}

	return Names;
}

void UEdAliasNode::Delete()
{
	this->Modify();

	const UEdGraphSchema* Schema = this->GetSchema();
	if (Schema != nullptr)
	{
		Schema->BreakNodeLinks(*this);
	}

	this->DestroyNode();
}

bool UEdAliasNode::Modify(bool bAlwaysMarkDirty)
{
	Super::Modify(bAlwaysMarkDirty);
	return this->GetGraph()->Modify(bAlwaysMarkDirty);
}

bool UEdAliasNode::HasEvent(FName InEvent)
{
	for (auto State : this->GetStateGraph()->GetStates())
	{
		if (this->AliasedStates.Contains(State->GetStateName()) != this->bComplement)
		{
			if (State->HasEvent(InEvent))
			{
				return true;
			}
		}
	}

	return this->GetStateGraph()->HasEvent(InEvent);;
}


#if WITH_EDITOR

void UEdAliasNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);


}

#endif

#undef LOCTEXT_NAMESPACE