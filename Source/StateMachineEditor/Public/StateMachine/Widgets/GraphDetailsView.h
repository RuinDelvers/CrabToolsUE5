#pragma once

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "StateMachine/EdGraph/EdEventObject.h"

#include "StateMachine/Editor.h"

class UStateMachineBlueprint;
class UEdStateGraph;
class UEdEventObject;
class UEdTransition;
class FGraphDetailsViewItem;

// Shortcuts for nestred templated types.
using ItemPtr = TSharedPtr<FGraphDetailsViewItem>;
using TablePtr = TSharedPtr<STreeView<ItemPtr>>;
using TableWeakPtr = TWeakPtr<STreeView<ItemPtr>>;

/*Base object to be put into the tree view. Used to generate the widgest for the TreeView.*/
class FGraphDetailsViewItem : public TSharedFromThis<FGraphDetailsViewItem>
{
private:
	TWeakPtr<FGraphDetailsViewItem> Parent;
	TableWeakPtr TableOwner;
	TArray<ItemPtr> Children;

public:

	virtual ~FGraphDetailsViewItem() {}
	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly);

	void Clear() { this->Children.Empty(); }
	void GetChildren(TArray<ItemPtr>& OutChildren) const;
	const TArray<ItemPtr>& ReadChildren() const { return this->Children; }
	void AddChild(ItemPtr Item, bool DeferRefresh = false);
	void RemoveChild(ItemPtr Item, bool DeferRefresh = false);
	void SetTableView(TableWeakPtr TableOwner);
	TWeakPtr<FGraphDetailsViewItem> GetParent() const { return this->Parent; }
	virtual void Select() {}
	virtual void InitView() {}
	virtual void Delete(bool DeferRefresh=false) {}
	virtual void EnterRenameMode() {}

	virtual void Sort();
	virtual FString SortKey() const { return ""; }
};

class FHeaderItem : public FGraphDetailsViewItem
{
private:

	FText HeaderText;

public:
	virtual ~FHeaderItem() {}
	FHeaderItem(FText Text)
		: HeaderText(Text) {}
	FHeaderItem(FString String)
		: HeaderText(FText::FromString(String)) {}

	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;

	FText GetHeaderText() const { return this->HeaderText; }
};

/* Used for Events and Statics to add an Add button to the header. */
class FCreateHeaderItem : public FHeaderItem
{
protected:

	TWeakObjectPtr<UEdStateGraph> GraphRef;

public:

	virtual ~FCreateHeaderItem() {}
	FCreateHeaderItem(UEdStateGraph* Graph, FText Text);
	FCreateHeaderItem(UEdStateGraph* Graph, FString String)
		: FCreateHeaderItem(Graph, FText::FromString(String)) {}

	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;

	FReply OnAddClicked() { return this->OnAddImplementation();  }
	virtual FReply OnAddImplementation() {  return FReply::Handled(); }
};

class FEventHeaderItem : public FCreateHeaderItem
{
	
public:
	
	FEventHeaderItem(UEdStateGraph* Graph, FText Text)
		: FCreateHeaderItem(Graph, Text) {}
	FEventHeaderItem(UEdStateGraph* Graph, FString Text)
		: FCreateHeaderItem(Graph, Text) {}
	

	virtual FReply OnAddImplementation() override;
};

class FBaseEditableTextItem : public FGraphDetailsViewItem
{
protected:
	TSharedPtr<SInlineEditableTextBlock> InlineText;

public:
	virtual void EnterRenameMode() override	{ this->InlineText->EnterEditingMode(); }
};

class FStateItem : public FBaseEditableTextItem
{
private:
	TWeakObjectPtr<UEdStateNode> NodeRef;

public:
	virtual ~FStateItem();
	FStateItem(UEdStateNode* NodePtr);

	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;
private:
	
	bool OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage);
	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
	void OnNameChanged(FName Old, FName Name);
	void OnNodeDeleted();
	bool IsReadOnly() const;

	void Select() override { this->NodeRef->Inspect(); }

	virtual void Delete(bool DeferRefresh = false) override;
	virtual FString SortKey() const { return this->NodeRef->GetNodeName().ToString(); }
};

class FStateMachineItem : public FBaseEditableTextItem
{
private:

	TWeakObjectPtr<UEdStateGraph> GraphRef;

public:
	virtual ~FStateMachineItem();
	FStateMachineItem(UEdStateGraph* GraphPtr);

	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;

	void AddState(UEdStateNode* State, bool DeferRefresh);

private:

	bool OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage);
	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
	void OnNameChanged(FName OldName, FName Name);
	void OnGraphDeleted();
	void OnEventCreated(UEdEventObject* Event, bool DeferRefresh);
	void OnGraphChanged(const FEdGraphEditAction& Action);

	bool IsReadOnly() const;

	void Select() override;
	virtual void Delete(bool DeferRefresh = false) override;

	virtual FString SortKey() const override;
};

class FEventItem : public FBaseEditableTextItem
{

protected:

	TWeakObjectPtr<UEdEventObject> EventReference;

public:
	FEventItem(TWeakObjectPtr<UEdEventObject> EventObject);

	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;

private:
	bool OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage);
	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

	void OnEventRenamed(FName From, FName To);
	void OnEventDeleted();

	void Select() override;

	virtual void Delete(bool DeferRefresh = false) override;

	virtual FString SortKey() const { return this->EventReference->GetEventName().ToString(); }
};

class FTransitionItem : public FGraphDetailsViewItem
{
private:
	TWeakObjectPtr<UEdTransition> NodeRef;

public:
	virtual ~FTransitionItem();
	FTransitionItem(UEdTransition* NodePtr);

	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;

private:
	void OnNodeDeleted();
};

/* Widget responsible for listing the details of individual state machines. */
class STATEMACHINEEDITOR_API SSubGraphDetails
: public SCompoundWidget, public FGCObject
{
private:
	TWeakObjectPtr<UEdStateGraph> GraphRef;
	TablePtr TreeView;

	ItemPtr StateRoot;
	ItemPtr EventRoot;
	
	// TreeView array.
	TArray<ItemPtr> TreeViewList;

	SLATE_BEGIN_ARGS(SSubGraphDetails) {}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, UEdStateGraph* GraphPtr);

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("SSubGraphDetails");
	}

	void AddState(UEdStateNode* Node, bool DeferRefresh);
	void AddEvent(UEdEventObject* Node, bool DeferRefresh);
	void Refresh();

private:
	void OnGraphDataReverted();
	void OnItemSelected(TSharedPtr< FGraphDetailsViewItem > InSelectedItem, ESelectInfo::Type SelectInfo) {}
	void OnItemDoubleClicked(TSharedPtr< FGraphDetailsViewItem > InClickedItem) {}
	void OnGetChildrenForCategory(ItemPtr InItem, TArray< ItemPtr >& OutChildren);
	void OnItemScrolledIntoView(ItemPtr InActionNode, const TSharedPtr<ITableRow>& InWidget) {}
	void OnSetExpansionRecursive(ItemPtr InTreeNode, bool bInIsItemExpanded) {}
	TSharedRef<ITableRow> OnGenerateRow(
		ItemPtr InItem,
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly);

	void OnSelectionChanged(
		ItemPtr SelectedItem,
		ESelectInfo::Type SelectInfo);

	void BindEvents();

	void OnStateAdded(UEdStateNode* State);
	void OnEventAdded(UEdEventObject* Event);

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;
};

/**
 * The tab which shows an overview of the State Machine graph, such as
 * Edges, Nodes, Events, or Aliases.
 */
class STATEMACHINEEDITOR_API SGraphDetailsView 
: public SCompoundWidget, public FGCObject
{
private:
	TWeakPtr<FEditor> EditorPtr;
	TSharedPtr<class SSearchBox> FilterBox;
	TablePtr TreeView;
	ItemPtr TreeViewRoot;

	//TArray<ItemPtr> TreeViewList;
	TSharedPtr<SWidgetSwitcher> DetailsTabs;
	TMap<UEdStateGraph*, TSharedPtr<SSubGraphDetails>> GraphToWidgetMap;

public:
	SLATE_BEGIN_ARGS(SGraphDetailsView){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedPtr<class FEditor> InEditor);

	virtual FString GetReferencerName() const override
	{
		return TEXT("SGraphDetailsView");
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;

private:
	/* Button handler for clicking on Add New StateMachine. */
	FReply OnAddStateMachine();
	void BindEvents(TSharedPtr<class FEditor> InEditor);
	void InitView(TSharedPtr<FEditor> InEditor);	
	void InitGraphDetailsView(UEdStateGraph* Graph, bool bDeferRefresh=false);

	// Event Handlers for State Machine changes.
	void OnGraphSelected(UEdStateGraph* Graph);
	void OnStateAdded(UEdStateNode* Node);

	void OnItemSelected(ItemPtr InSelectedItem, ESelectInfo::Type SelectInfo) {}
	void OnItemDoubleClicked(ItemPtr InClickedItem) {}
	void OnGetChildrenForCategory(ItemPtr InItem, TArray< ItemPtr >& OutChildren);
	void OnItemScrolledIntoView(ItemPtr InActionNode, const TSharedPtr<ITableRow>& InWidget) {}
	void OnSetExpansionRecursive(ItemPtr InTreeNode, bool bInIsItemExpanded) {}


	// Section Functions.
	TSharedRef<ITableRow> OnGenerateRow(
		ItemPtr InItem,
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly);

	

	void OnSelectionChanged(
		ItemPtr SelectedItem,
		ESelectInfo::Type SelectInfo);
};