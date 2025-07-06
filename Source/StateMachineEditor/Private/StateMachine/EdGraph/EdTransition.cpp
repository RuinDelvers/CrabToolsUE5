#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdAliasNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"


#define LOCTEXT_NAMESPACE "EdNode_GenericGraphEdge"

UEdTransition::UEdTransition()
{
	bCanRenameNode = true;
}


void UEdTransition::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Edge"), FName(), TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Edge"), FName(), TEXT("Out"));
	Outputs->bHidden = true;
}

FText UEdTransition::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText();
}

void UEdTransition::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		// Commit suicide; transitions must always have an input and output connection
		Modify();

		// Our parent graph will have our graph in SubGraphs so needs to be modified to record that.
		if (UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

void UEdTransition::PrepareForCopying()
{
	
}

void UEdTransition::CreateConnections(UEdBaseStateNode* Start, UEdBaseStateNode* End)
{
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	Start->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(Start->GetOutputPin());

	// This to next
	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	End->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(End->GetInputPin());
}

UEdBaseStateNode* UEdTransition::GetStartNode() const
{
	if (Pins[0]->LinkedTo.Num() > 0)
	{
		return Cast<UEdBaseStateNode>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}

UEdBaseStateNode* UEdTransition::GetEndNode() const
{
	if (Pins[1]->LinkedTo.Num() > 0)
	{
		return Cast<UEdBaseStateNode>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}

TArray<FString> UEdTransition::GetEventOptions() const
{
	TSet<FString> EventNames;

	for (const auto& EName : Cast<UEdStateGraph>(this->GetGraph())->GetEventOptions())
	{
		EventNames.Add(EName);
	}

	EventNames.Append(this->GetStartNode()->GetEventOptions());

	EventNames.Sort([&](const FString& A, const FString& B) { return A < B; });

	return EventNames.Array();
}

TMap<FName, FTransitionData> UEdTransition::GetTransitionData(FNodeVerificationContext& Context)
{
	TMap<FName, FTransitionData> Data;

	for (auto& Values : this->EventToConditionMap)
	{
		auto Node = this->GetEndNode();

		if (auto StateNode = Cast<UEdStateNode>(Node))
		{
			FTransitionData DataValue
			{
				StateNode->GetStateName(),
				DuplicateObject(Values.Value.Condition, Context.GetOuter()),
				DuplicateObject(Values.Value.DataCondition, Context.GetOuter()),
			};

			if (!this->GetStartNode()->HasEvent(Values.Key))
			{
				auto StartName = this->GetStartNode()->GetStateName();
				auto EndName = StateNode->GetStateName();
				auto Msg = FString::Printf(TEXT("Transition from %s to %s uses unknown event: %s"),
					*StartName.ToString(),
					*EndName.ToString(),
					*Values.Key.ToString());

				Context.Error(Msg, this);

				continue;
			}

			Data.Add(Values.Key, DataValue);
		}
		else if (auto AliasNode = Cast<UEdAliasNode>(Node))
		{
			for (auto& SName : AliasNode->GetAliasedStates())
			{
				FTransitionData DataValue
				{
					SName,
					DuplicateObject(Values.Value.Condition, Context.GetOuter()),
					DuplicateObject(Values.Value.DataCondition, Context.GetOuter()),
				};

				if (!this->GetStartNode()->HasEvent(Values.Key))
				{
					auto StartName = this->GetStartNode()->GetStateName();
					auto EndName = SName;
					auto Msg = FString::Printf(TEXT("Transition from %s to %s uses unknown event: %s"),
						*StartName.ToString(),
						*EndName.ToString(),
						*Values.Key.ToString());

					Context.Error(Msg, this);

					continue;
				}

				Data.Add(Values.Key, DataValue);
			}
		}
	}

	return Data;
}

void UEdTransition::Delete()
{
	this->Modify();

	const UEdGraphSchema* Schema = this->GetSchema();
	if (Schema != nullptr)
	{
		Schema->BreakNodeLinks(*this);
	}

	this->DestroyNode();
}

bool UEdTransition::Modify(bool bAlwaysMarkDirty)
{
	Super::Modify(bAlwaysMarkDirty);
	return this->GetGraph()->Modify(bAlwaysMarkDirty);
}

#undef LOCTEXT_NAMESPACE

