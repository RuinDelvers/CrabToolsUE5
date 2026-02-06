#pragma once

#include "Layout/Geometry.h"
#include "Input/Reply.h"
#include "UObject/GCObject.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "StateMachine/Editor.h"
#include "GraphEditor.h"

class UStateMachineBlueprint;
class UEdBaseStateNode;

class STATEMACHINEEDITOR_API SGraphView : public SCompoundWidget, public FGCObject
{

private:
	TSharedPtr<FUICommandList> GraphEditorCommands;
	TWeakObjectPtr<UStateMachineBlueprint> BlueprintRef;
	TArray<TWeakObjectPtr<UEdGraphNode>> SelectedNodes;
	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<SWidgetSwitcher> TabsWidget;
	TMap<TWeakObjectPtr<UEdGraph>, TSharedPtr<SGraphEditor>> GraphToEditorMap;

public:
	SLATE_BEGIN_ARGS( SGraphView ) {}
	SLATE_END_ARGS()


	void Construct(
		const FArguments& InArgs, 
		TSharedPtr<class FEditor> InBlueprintEditor);

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual FString GetReferencerName() const override
	{
		return TEXT("SGraphView");
	}

	virtual FReply OnKeyDown(const FGeometry& Geo, const FKeyEvent& Event) override;

private:
	void OnSelectionChanged(const TSet<class UObject*>& NewSelection);
	void OnGraphSelected(UEdStateGraph* Graph);
	bool CanDeleteNodes() const;
	void OnDeleteNodes();
	bool CanCopyNodes() const;
	void OnCopyNodes();

	bool CanCutNodes() const;
	void OnCutNodes();

	bool CanDuplicateNodes() const;
	void OnDuplicateNodes();
	
	void OnRenameSelection();
	bool CanRenameSelection();
	void BindEvents(UStateMachineBlueprint* Blueprint);

	void AddGraphToEditor(UEdGraph* Graph);
};