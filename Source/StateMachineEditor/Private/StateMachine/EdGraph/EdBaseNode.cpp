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

void UEdBaseNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdBaseNode, Comment))
	{
		this->Events.OnCommentChanged.Broadcast(this->Comment);
	}
}

void UEdBaseStateNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"));
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"));
}

void UEdBaseStateNode::GetTransitionList(TArray<UEdTransition*>& OutTransitions) const
{
	// Normal transitions
	for (int32 LinkIndex = 0; LinkIndex < Pins[1]->LinkedTo.Num(); ++LinkIndex)
	{
		UEdGraphNode* TargetNode = Pins[1]->LinkedTo[LinkIndex]->GetOwningNode();
		if (UEdTransition* Transition = Cast<UEdTransition>(TargetNode))
		{
			OutTransitions.Add(Transition);
		}
	}

	// Bidirectional transitions where we are the 'backwards' link.
	// Conduits and other states types that don't support bidirectional transitions should hide it from the details panel.
	for (int32 LinkIndex = 0; LinkIndex < Pins[0]->LinkedTo.Num(); ++LinkIndex)
	{
		UEdGraphNode* TargetNode = Pins[0]->LinkedTo[LinkIndex]->GetOwningNode();
		if (UEdTransition* Transition = Cast<UEdTransition>(TargetNode))
		{
			// Anim state nodes that don't support bidirectional transitions should hide this property in FAnimTransitionNodeDetails::CustomizeDetails
			//if (Transition->Bidirectional)
			//{
			//	OutTransitions.Add(Transition);
			//}
		}
	}

	// Sort the transitions by priority order, lower numbers are higher priority
	/*
	if (bWantSortedList)
	{
		struct FCompareTransitionsByPriority
		{
			FORCEINLINE bool operator()(const UEdTransition& A, const UEdTransition& B) const
			{
				return A.PriorityOrder < B.PriorityOrder;
			}
		};

		OutTransitions.Sort(FCompareTransitionsByPriority());
	}
	*/
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

IStateMachineLike* UEdBaseStateNode::GetMachineLike() const
{
	return this->GetStateGraph();
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