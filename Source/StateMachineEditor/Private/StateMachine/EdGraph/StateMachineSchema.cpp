#include "StateMachine/EdGraph/StateMachineSchema.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"
#include "StateMachine/EdGraph/EdAliasNode.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/EdGraph/StateConnectionDrawingPolicy.h"

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

FString FSMSchemaAction_NewExtensionNode::GetReferencerName() const
{
	return "FSMSchemaAction_NewExtensionNode";
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

FString FSMSchemaAction_NewAliasNode::GetReferencerName() const
{
	return "FSMSchemaAction_NewAliasNode";
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

FString FSMSchemaAction_NewNode::GetReferencerName() const
{
	return "FSMSchemaAction_NewNode";
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

FString FSMSchemaAction_NewEdge::GetReferencerName() const
{
	return "FSMSchemaAction_NewEdge";
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
		bool c1 = (*It)->IsChildOf(UStateNode::StaticClass());
		bool c2 = !It->HasAnyClassFlags(CLASS_Abstract);
		bool c3 = !Visited.Contains(*It);

		if ( c1 && c2 && c3)
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
}

void UStateMachineSchema::AddEmptyAction(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const FText Desc = LOCTEXT("NewStateGraphExtensionNodeMenuText", "Create Empty State");
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

#pragma endregion

#undef LOCTEXT_NAMESPACE