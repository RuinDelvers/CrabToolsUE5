#include "StateMachine/EdGraph/StateMachineSchema.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"
#include "StateMachine/EdGraph/EdAliasNode.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/EdGraph/StateConnectionDrawingPolicy.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "Framework/Commands/GenericCommands.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraphNode_Comment.h"

#define LOCTEXT_NAMESPACE "StateMachineSchema"

int32 UStateMachineSchema::CurrentCacheRefreshID = 0;

namespace Helpers
{
	// Helper function to format the right-click context menu of the graph editor.
	static FText GetFormattedMenuText(TSubclassOf<UStateNode> NodeType)
	{
		FString ClassString = NodeType->GetMetaData("Category");

		if (ClassString.StartsWith("StateMachine|"))
		{
			ClassString.RightChopInline(13);
		}
		else if (ClassString.StartsWith("StateMachine"))
		{
			ClassString.RightChopInline(12);
		}

		return FText::Format(LOCTEXT("StateMAchineGraphNodeAction", "{0}"), FText::FromString(ClassString));
	}
}

UEdGraphNode* FSMSchemaAction_NewExtensionNode::PerformAction(
	class UEdGraph* ParentGraph,
	UEdGraphPin* FromPin,
	const FVector2D Location,
	bool bSelectNewNode)
{
	UEdStateNode* ResultNode = nullptr;
	UEdStateGraph* StateGraph = Cast<UEdStateGraph>(ParentGraph);

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(
			LOCTEXT("StateMachineEditorNewExtensionNode", "State Machine Editor: New Extension Node"));

		ParentGraph->Modify();
		if (FromPin != nullptr)
			FromPin->Modify();

		NodeTemplate->Rename(nullptr, ParentGraph);

		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

void FSMSchemaAction_NewExtensionNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

UEdGraphNode* FSMSchemaAction_NewAliasNode::PerformAction(
	class UEdGraph* ParentGraph,
	UEdGraphPin* FromPin,
	const FVector2D Location,
	bool bSelectNewNode)
{
	UEdAliasNode* ResultNode = nullptr;
	UEdStateGraph* StateGraph = Cast<UEdStateGraph>(ParentGraph);

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(
			LOCTEXT("StateMachineEditorNewAliasNode", "State Machine Editor: New Alias Node"));

		ParentGraph->Modify();
		if (FromPin != nullptr)
			FromPin->Modify();

		NodeTemplate->Rename(nullptr, ParentGraph);

		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

void FSMSchemaAction_NewAliasNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

UEdGraphNode* FSMSchemaAction_NewNode::PerformAction(
	class UEdGraph* ParentGraph, 
	UEdGraphPin* FromPin, 
	const FVector2D Location, 
	bool bSelectNewNode)
{
	UEdStateNode* ResultNode = nullptr;
	UEdStateGraph* StateGraph = Cast<UEdStateGraph>(ParentGraph);

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(
			LOCTEXT("StateMachineEditorNewNode", "State Machine Editor: New Node"));

		ParentGraph->Modify();
		if (FromPin != nullptr)
			FromPin->Modify();


		NodeTemplate->Rename(nullptr, ParentGraph);
		NodeTemplate->SetStateName(StateGraph->GetNewStateName());
		
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;

		if (this->NodeClass)
		{
			ResultNode->SetNodeTemplate(NewObject<UStateNode>(
				ResultNode,
				this->NodeClass,
				FName(FString("StateNode")),
				RF_NoFlags
			));
		}
	}

	return ResultNode;
}

void FSMSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

UEdGraphNode* FSMSchemaAction_NewEdge::PerformAction(
	class UEdGraph* ParentGraph,
	UEdGraphPin* FromPin,
	const FVector2D Location,
	bool bSelectNewNode)
{
	UEdTransition* ResultNode = nullptr;
	UEdStateGraph* StateGraph = Cast<UEdStateGraph>(ParentGraph);

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(
			LOCTEXT("StateMachineEditorNewNode", "State Machine Editor: New Node"));

		ParentGraph->Modify();
		if (FromPin != nullptr)
			FromPin->Modify();


		NodeTemplate->Rename(nullptr, ParentGraph);

		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

void FSMSchemaAction_NewEdge::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

UEdGraphNode* FSMSchemaAction_NewComment::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode_Comment* ResultNode = nullptr;
	UEdStateGraph* StateGraph = Cast<UEdStateGraph>(ParentGraph);

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(
			LOCTEXT("StateMachineEditorNewNode", "State Machine Editor: New Node"));

		ParentGraph->Modify();
		if (FromPin != nullptr)
			FromPin->Modify();


		NodeTemplate->Rename(nullptr, ParentGraph);

		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

void FSMSchemaAction_NewComment::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

#pragma region Schema

UStateMachineSchema::UStateMachineSchema(
	const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}


void UStateMachineSchema::BackwardCompatibilityNodeConversion(UEdGraph* Graph, bool bOnlySafeChanges) const
{

}

EGraphType UStateMachineSchema::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_StateMachine;
}



void UStateMachineSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const bool bNoParent = (ContextMenuBuilder.FromPin == NULL);

	const FText AddToolTip = LOCTEXT("NewStateGraphNodeTooltip", "Add node here");

	TSet<TSubclassOf<UStateNode> > Visited;

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		bool c1 = Class->IsChildOf(UStateNode::StaticClass());
		bool c2 = !Class->IsChildOf<UAbstractCondition>();
		bool c3 = !It->HasAnyClassFlags(CLASS_Abstract);
		bool c4 = !Visited.Contains(*It);

		if ( c1 && c2 && c3 && c4)
		{
			TSubclassOf<UStateNode> NodeType = *It;

			if (It->GetName().StartsWith("REINST") || It->GetName().StartsWith("SKEL"))
				continue;

			FString Title = NodeType->GetName();
			Title.RemoveFromEnd("_C");
			FText Desc = FText::FromString(Title);
			
			FText MenuText = Helpers::GetFormattedMenuText(NodeType);
			
			TSharedPtr<FSMSchemaAction_NewNode> Action(
				new FSMSchemaAction_NewNode(MenuText, Desc, AddToolTip, 0));
			Action->SetNodeClass(NodeType);

			Action->SetNodeTemplate(NewObject<UEdStateNode>(ContextMenuBuilder.OwnerOfTemporaries));

			ContextMenuBuilder.AddAction(Action);

			Visited.Add(NodeType);
		}
	}

	this->AddEmptyAction(ContextMenuBuilder);
	this->AddExtensionAction(ContextMenuBuilder);
	this->AddAliasAction(ContextMenuBuilder);
	this->AddCommentAction(ContextMenuBuilder);
}

void UStateMachineSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (auto StateNode = Cast<UEdBaseNode>(Context->Node))
	{
		FToolMenuInsert Insert;
		Insert.Position = EToolMenuInsertType::First;

		FToolMenuSection& Section = Menu->AddSection(
			"StateMachineGraphSchemaNodeActions",
			LOCTEXT("ClassActionsMenuHeader", "Edit"),
			Insert);

		Section.AddMenuEntry(FGenericCommands::Get().Delete);
		Section.AddMenuEntry(FGenericCommands::Get().Cut);
		Section.AddMenuEntry(FGenericCommands::Get().Copy);
		Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
		Section.AddMenuEntry(FGenericCommands::Get().Rename);

	}

	Super::GetContextMenuActions(Menu, Context);
}

void UStateMachineSchema::AddEmptyAction(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const FText Desc = LOCTEXT("NewStateGraphExtensionNodeMenuText", "Create Empty State...");
	const FText MenuText = FText::FromString("");
	const FText ToolTip = LOCTEXT("NewStateGraphNodeTooltip", "Add empty state here");

	TSharedPtr<FSMSchemaAction_NewNode> Action(
		new FSMSchemaAction_NewNode(MenuText, Desc, ToolTip, -100));
	Action->SetNodeClass(nullptr);

	Action->SetNodeTemplate(NewObject<UEdStateNode>(ContextMenuBuilder.OwnerOfTemporaries));

	auto StateNode = NewObject<UEdStateNode>(ContextMenuBuilder.OwnerOfTemporaries);
	Action->SetNodeTemplate(StateNode);

	ContextMenuBuilder.AddAction(Action);
}

void UStateMachineSchema::AddExtensionAction(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const FText Desc = LOCTEXT("NewStateGraphExtensionNodeMenuText", "Extend an existing node...");
	const FText MenuText = FText::FromString("");
	const FText ToolTip = LOCTEXT("NewStateGraphNodeTooltip", "Add extension node here");

	TSharedPtr<FSMSchemaAction_NewExtensionNode> ExtendAction(
		new FSMSchemaAction_NewExtensionNode(MenuText, Desc, ToolTip, -100));
	
	auto StateNode = NewObject<UEdStateNode>(ContextMenuBuilder.OwnerOfTemporaries);
	StateNode->SetNodeType(EStateNodeType::EXTENDED_NODE);
	ExtendAction->SetNodeTemplate(StateNode);

	ContextMenuBuilder.AddAction(ExtendAction);
}

void UStateMachineSchema::AddAliasAction(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const FText Desc = LOCTEXT("NewStateGraphExtensionNodeMenuText", "Alias existing nodes...");
	const FText MenuText = FText::FromString("");
	const FText ToolTip = LOCTEXT("NewStateGraphNodeTooltip", "Add an alias node here");

	TSharedPtr<FSMSchemaAction_NewAliasNode> ExtendAction(
		new FSMSchemaAction_NewAliasNode(MenuText, Desc, ToolTip, -100));

	auto StateNode = NewObject<UEdAliasNode>(ContextMenuBuilder.OwnerOfTemporaries);
	ExtendAction->SetNodeTemplate(StateNode);

	ContextMenuBuilder.AddAction(ExtendAction);
}

void UStateMachineSchema::AddCommentAction(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const FText Desc = LOCTEXT("NewStateGraphExtensionNodeMenuText", "Create a Commented Section...");
	const FText MenuText = FText::FromString("");
	const FText ToolTip = LOCTEXT("NewStateGraphNodeTooltip", "Add comment section here...");

	TSharedPtr<FSMSchemaAction_NewComment> ExtendAction(
		new FSMSchemaAction_NewComment(MenuText, Desc, ToolTip, -1000));

	auto StateNode = NewObject<UEdGraphNode_Comment>(ContextMenuBuilder.OwnerOfTemporaries);
	ExtendAction->SetNodeTemplate(StateNode);

	ContextMenuBuilder.AddAction(ExtendAction);
}

FConnectionDrawingPolicy* UStateMachineSchema::CreateConnectionDrawingPolicy(
	int32 InBackLayerID, 
	int32 InFrontLayerID, 
	float InZoomFactor, 
	const FSlateRect& InClippingRect, 
	class FSlateWindowElementList& InDrawElements, 
	class UEdGraph* InGraphObj) const
{
	return new FStateMachineConnectionDrawingPolicy(
		InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}


bool UStateMachineSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const
{
	auto NodeA = Cast<UEdBaseStateNode>(A->GetOwningNode());
	auto NodeB = Cast<UEdBaseStateNode>(B->GetOwningNode());

	// Are nodes and pins all valid?
	if (!NodeA || !NodeA->GetOutputPin() || !NodeB || !NodeB->GetInputPin())
	{
		return false;
	}
	
	FVector2D InitPos((NodeA->NodePosX + NodeB->NodePosX) / 2, (NodeA->NodePosY + NodeB->NodePosY) / 2);

	FSMSchemaAction_NewEdge Action;
	Action.SetNodeTemplate(NewObject<UEdTransition>(NodeA->GetGraph()));
	UEdTransition* EdgeNode = Cast<UEdTransition>(Action.PerformAction(NodeA->GetGraph(), nullptr, InitPos, false));

	// Always create connections from node A to B, don't allow adding in reverse
	EdgeNode->CreateConnections(NodeA, NodeB);

	return true;
}

const FPinConnectionResponse UStateMachineSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// Make sure the pins are not on the same node
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameNode", "Can't connect node to itself"));
	}

	const UEdGraphPin* Out = A;
	const UEdGraphPin* In = B;

	UEdBaseStateNode* EdNode_Out = Cast<UEdBaseStateNode>(Out->GetOwningNode());
	UEdBaseStateNode* EdNode_In = Cast<UEdBaseStateNode>(In->GetOwningNode());

	auto EdGraph = Cast<UEdStateGraph>(Out->GetOwningNode()->GetGraph());
	check(EdGraph);

	if (EdNode_Out == nullptr || EdNode_In == nullptr)
	{
		return FPinConnectionResponse(
			CONNECT_RESPONSE_DISALLOW, 
			LOCTEXT("PinError", "Invalid Node Connection."));
	}
	else if (auto StartNode = Cast<UEdStartStateNode>(EdNode_Out))
	{
		auto Destinations = EdGraph->GetDestinations(EdNode_Out);

		if (EdGraph->CanOverrideStart())
		{
			if (Destinations.Num() > 0)
			{
				return FPinConnectionResponse(
					CONNECT_RESPONSE_DISALLOW,
					LOCTEXT("PinError", "Cannot link Start State to multiple other nodes."));
			}
			else
			{
				return FPinConnectionResponse(
					CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE,
					LOCTEXT("PinConnect", "Connect nodes with edge"));
			}
		}
		else
		{
			return FPinConnectionResponse(
				CONNECT_RESPONSE_DISALLOW,
				LOCTEXT("PinError", "Overriding of Starting State is not allowed for this graph."));
		}
	}
	else if (Cast<UEdStartStateNode>(EdNode_In))
	{
		return FPinConnectionResponse(
			CONNECT_RESPONSE_DISALLOW, 
			LOCTEXT("PinError", "Cannot connect to the StartNode as a Destination."));
	}

	return FPinConnectionResponse(
		CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, 
		LOCTEXT("PinConnect", "Connect nodes with edge"));
}

bool UStateMachineSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	// We don't actually care about the pin, we want the node that is being dragged between
	auto NodeA = Cast<UEdBaseStateNode>(A->GetOwningNode());
	auto NodeB = Cast<UEdBaseStateNode>(B->GetOwningNode());

	// Check that this edge doesn't already exist
	for (UEdGraphPin* TestPin : NodeA->GetOutputPin()->LinkedTo)
	{
		UEdGraphNode* ChildNode = TestPin->GetOwningNode();
		if (UEdTransition* EdNode_Edge = Cast<UEdTransition>(ChildNode))
		{
			ChildNode = EdNode_Edge->GetEndNode();
		}

		if (ChildNode == NodeB)
			return false;
	}

	if (NodeA && NodeB)
	{
		// Always create connections from node A to B, don't allow adding in reverse
		Super::TryCreateConnection(NodeA->GetOutputPin(), NodeB->GetInputPin());
		return true;
	}
	else
	{
		return false;
	}
}

void UStateMachineSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<UEdStartStateNode> NodeCreator(Graph);
	auto ResultStartNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();

	this->SetNodeMetaData(ResultStartNode, FNodeMetadata::DefaultGraphNode);
}

bool UStateMachineSchema::SupportsDropPinOnNode(
	UEdGraphNode* InTargetNode, 
	const FEdGraphPinType& InSourcePinType, 
	EEdGraphPinDirection InSourcePinDirection, 
	FText& OutErrorMessage) 
	const
{
	return Cast<UEdBaseStateNode>(InTargetNode) != nullptr;
}

UEdGraphPin* UStateMachineSchema::DropPinOnNode(
	UEdGraphNode* InTargetNode, 
	const FName& InSourcePinName, 
	const FEdGraphPinType& InSourcePinType, 
	EEdGraphPinDirection InSourcePinDirection) 
	const
{
	auto EdNode = Cast<UEdBaseStateNode>(InTargetNode);
	switch (InSourcePinDirection)
	{
		case EGPD_Input:
			return EdNode->GetOutputPin();
		case EGPD_Output:
			return EdNode->GetInputPin();
		default:
			return nullptr;
	}
}

FLinearColor UStateMachineSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::White;
}

void UStateMachineSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakNodeLinks", "Break Node Links"));

	Super::BreakNodeLinks(TargetNode);
}

void UStateMachineSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UStateMachineSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));

	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

bool UStateMachineSchema::IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UStateMachineSchema::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UStateMachineSchema::ForceVisualizationCacheClear() const
{
	++CurrentCacheRefreshID;
}

bool UStateMachineSchema::IsConnectionRelinkingAllowed(UEdGraphPin* InPin) const
{
	if (InPin && InPin->GetOwningNode())
	{
		auto StateNode = Cast<UEdBaseStateNode>(InPin->GetOwningNode());
		auto TransitionNode = Cast<UEdTransition>(InPin->GetOwningNode());

		if (StateNode || TransitionNode)
		{
			return true;
		}
	}

	return false;
}

bool UStateMachineSchema::TryRelinkConnectionTarget(UEdGraphPin* SourcePin, UEdGraphPin* OldTargetPin, UEdGraphPin* NewTargetPin, const TArray<UEdGraphNode*>& InSelectedGraphNodes) const
{
	const FPinConnectionResponse Response = CanCreateConnection(SourcePin, NewTargetPin);
	if (Response.Response == ECanCreateConnectionResponse::CONNECT_RESPONSE_DISALLOW)
	{
		return false;
	}

	bool bRelinkHead = SourcePin->Direction == EGPD_Output;
	auto OldTargetState = Cast<UEdBaseStateNode>(OldTargetPin->GetOwningNode());
	auto NewTargetState = Cast<UEdBaseStateNode>(NewTargetPin->GetOwningNode());

	if (OldTargetState == nullptr || OldTargetState->GetInputPin() == nullptr || OldTargetState->GetOutputPin() == nullptr ||
		NewTargetState == nullptr || NewTargetState->GetInputPin() == nullptr || NewTargetState->GetOutputPin() == nullptr)
	{
		return false;
	}

	// In the case we are relinking the transition starting at the entry state, the SourceState is nullptr. Special case handling.
	auto EntryState = Cast<UEdStartStateNode>(SourcePin->GetOwningNode());
	if (EntryState)
	{
		// Remove the incoming transition from the previous target state
		OldTargetPin->Modify();
		OldTargetPin->LinkedTo.Remove(SourcePin);
		SourcePin->Modify();
		SourcePin->LinkedTo.Remove(OldTargetPin);

		// Add the new incoming transition to the new target state
		TryCreateConnection(SourcePin, NewTargetPin);

		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(EntryState->GetGraph());
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

		return true;
	}

	// Collect all transition nodes starting at the source state, filter them by the transitions and perform the actual relink operation.
	const TArray<UEdTransition*> TransitionNodes = UEdTransition::GetListTransitionNodesToRelink(SourcePin, OldTargetPin, InSelectedGraphNodes);

	if (bRelinkHead)
	{
		for (UEdTransition* TransitionNode : TransitionNodes)
		{
			TransitionNode->RelinkHead(NewTargetState);
		}
	}
	else
	{
		for (UEdTransition* TransitionNode : TransitionNodes)
		{
			TransitionNode->RelinkTail(NewTargetState);
		}
	}

	// In case one or more transitions got relinked, inform the blueprint about the changes
#if WITH_EDITOR
	if (!TransitionNodes.IsEmpty())
	{
		//UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(OneRelinkedTransition->GetBoundGraph());
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(TransitionNodes[0]->GetStateGraph());
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);


		SourcePin->GetOwningNode()->PinConnectionListChanged(SourcePin);
		OldTargetPin->GetOwningNode()->PinConnectionListChanged(OldTargetPin);
		NewTargetPin->GetOwningNode()->PinConnectionListChanged(NewTargetPin);
	}
#endif//#if WITH_EDITOR

	return true;
}

const FPinConnectionResponse UStateMachineSchema::CanRelinkConnectionToPin(const UEdGraphPin* OldSourcePin, const UEdGraphPin* TargetPinCandidate) const
{
	FPinConnectionResponse Response = CanCreateConnection(OldSourcePin, TargetPinCandidate);
	if (Response.Response != CONNECT_RESPONSE_DISALLOW)
	{
		Response.Message = FText::FromString("Relink transition");
	}

	return Response;
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
