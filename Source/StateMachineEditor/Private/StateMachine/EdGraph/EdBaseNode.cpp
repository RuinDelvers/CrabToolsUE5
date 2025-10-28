#include "StateMachine/EdGraph/EdBaseNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/StateMachineBlueprint.h"

void UEdBaseNode::Inspect()
{
	if (auto Graph = Cast<UEdStateGraph>(this->GetGraph()))
	{
		Graph->GetBlueprintOwner()->Events.OnObjectInspected.Broadcast(this);
	}
}

void UEdBaseStateNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"));
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"));
}

void UEdBaseStateNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

void UEdBaseNode::ClearEvents()
{
	this->Events.OnNameChanged.Clear();
	this->Events.OnNodeDeleted.Clear();
}

UEdStateGraph* UEdBaseNode::GetStateGraph() const
{ 
	return Cast<UEdStateGraph>(this->GetGraph());
}

TArray<FString> UEdBaseStateNode::GetEnterStates() const
{
	TArray<FString> Names;

	for (auto Pin : this->Pins)
	{
		if (Pin->LinkedTo.Num() > 0)
		{
			for (auto LinkedTo : Pin->LinkedTo)
			{
				if (auto CastNode = Cast<UEdTransition>(LinkedTo->GetOwningNode()))
				{
					if (CastNode->GetEndNode() == this)
					{
						if (auto CastStateNode = Cast<UEdStateNode>(CastNode->GetStartNode()))
						{
							Names.Add(CastStateNode->GetStateName().ToString());
						}
					}
				}
			}
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdBaseStateNode::GetExitStates() const
{
	TArray<FString> Names;

	for (auto Pin : this->Pins)
	{
		if (Pin->LinkedTo.Num() > 0)
		{
			for (auto LinkedTo : Pin->LinkedTo)
			{
				if (auto CastNode = Cast<UEdTransition>(LinkedTo->GetOwningNode()))
				{
					if (CastNode->GetStartNode() == this)
					{
						if (auto CastStateNode = Cast<UEdStateNode>(CastNode->GetEndNode()))
						{
							Names.Add(CastStateNode->GetStateName().ToString());
						}
					}
				}
			}
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

void UEdBaseStateNode::OnModify()
{
	this->GetStateGraph()->OnModify();
}