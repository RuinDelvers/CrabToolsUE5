#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraphSchema_K2.h"
#include "StateMachineSchema.generated.h"

class UEdAliasNode;
class UEdStateNode;
class UEdExtension;
class UEdTransition;

USTRUCT()
struct STATEMACHINEEDITOR_API FSMSchemaAction_NewExtensionNode: public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

private:

	TObjectPtr<UEdStateNode> NodeTemplate;

public:
	FSMSchemaAction_NewExtensionNode() : NodeTemplate(nullptr)
	{
		this->Grouping = -100;
	}

	FSMSchemaAction_NewExtensionNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr)
	{
	}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

	void SetNodeTemplate(UEdStateNode* Template) { this->NodeTemplate = Template; }

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

USTRUCT()
struct STATEMACHINEEDITOR_API FSMSchemaAction_NewAliasNode: public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

private:

	TObjectPtr<UEdAliasNode> NodeTemplate;

public:
	FSMSchemaAction_NewAliasNode() : NodeTemplate(nullptr)
	{
		this->Grouping = -100;
	}

	FSMSchemaAction_NewAliasNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr)
	{
	}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

	void SetNodeTemplate(UEdAliasNode* Template) { this->NodeTemplate = Template; }

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

USTRUCT()
struct STATEMACHINEEDITOR_API FSMSchemaAction_NewNode: public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

private:
	TSubclassOf<class UStateNode> NodeClass;
	TObjectPtr<UEdStateNode> NodeTemplate;

public:
	FSMSchemaAction_NewNode(): NodeTemplate(nullptr) {}

	FSMSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr) {}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	void SetNodeClass(TSubclassOf<UStateNode> Class) { this->NodeClass = Class; }
	void SetNodeTemplate(UEdStateNode* Template) { this->NodeTemplate = Template; }

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

USTRUCT()
struct STATEMACHINEEDITOR_API FSMSchemaAction_NewEdge: public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

private:

	TObjectPtr<UEdTransition> NodeTemplate;

public:
	FSMSchemaAction_NewEdge() : NodeTemplate(nullptr) {}

	FSMSchemaAction_NewEdge(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr)
	{
	}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	//virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	void SetNodeTemplate(UEdTransition* Template) { this->NodeTemplate = Template; }

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

UCLASS(MinimalAPI)
class UStateMachineSchema : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()

	static int32 CurrentCacheRefreshID;

public:
	virtual void BackwardCompatibilityNodeConversion(
		UEdGraph* Graph, bool bOnlySafeChanges) const override;

	virtual EGraphType GetGraphType(const UEdGraph* TestEdGraph) const override;
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(
		int32 InBackLayerID, 
		int32 InFrontLayerID, 
		float InZoomFactor, 
		const FSlateRect& InClippingRect, 
		class FSlateWindowElementList& InDrawElements, 
		class UEdGraph* InGraphObj) 
		const override;

	virtual bool CreateAutomaticConversionNodeAndConnections(
		UEdGraphPin* A, 
		UEdGraphPin* B) 
		const override;

	virtual const FPinConnectionResponse CanCreateConnection(
		const UEdGraphPin* A, const UEdGraphPin* B) const;

	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual bool SupportsDropPinOnNode(
		UEdGraphNode* InTargetNode, 
		const FEdGraphPinType& InSourcePinType, 
		EEdGraphPinDirection InSourcePinDirection, 
		FText& OutErrorMessage) 
		const override;

	UEdGraphPin* DropPinOnNode(
		UEdGraphNode* InTargetNode,
		const FName& InSourcePinName,
		const FEdGraphPinType& InSourcePinType,
		EEdGraphPinDirection InSourcePinDirection)
		const;

	FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const;
	void BreakNodeLinks(UEdGraphNode& TargetNode) const;
	void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const;
	void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const;

	virtual bool IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const override;
	virtual int32 GetCurrentVisualizationCacheID() const override;
	virtual void ForceVisualizationCacheClear() const override;

private:

	void AddExtensionAction(FGraphContextMenuBuilder& ContextMenuBuilder) const;
	void AddAliasAction(FGraphContextMenuBuilder& ContextMenuBuilder) const;
	void AddEmptyAction(FGraphContextMenuBuilder& ContextMenuBuilder) const;
};
