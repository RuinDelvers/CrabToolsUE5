#include "StateMachine/Widgets/GraphDetailsView.h"

#include "Widgets/SWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Widgets/Text/SRichTextBlock.h"

#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/EdGraph/EdEventObject.h"
#include "GraphEditAction.h"
#include "StateMachine/StateMachineBlueprint.h"

#define LOCTEXT_NAMESPACE "PSM"

class SCategoryHeaderTableRow : public STableRow<ItemPtr>
{
private:
	TSharedPtr<SBorder> ContentBorder;

public:

	SLATE_BEGIN_ARGS(SCategoryHeaderTableRow) {}
		SLATE_DEFAULT_SLOT(typename SCategoryHeaderTableRow::FArguments, Content)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		STableRow<ItemPtr>::ChildSlot
			.Padding(0.0f, 2.0f, .0f, 0.0f)
			[
				SAssignNew(ContentBorder, SBorder)
				.BorderImage(this, &SCategoryHeaderTableRow::GetBackgroundImage)
				.Padding(FMargin(3.0f, 5.0f))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.Padding(5.0f)
					.AutoWidth()
					[
						SNew(SExpanderArrow, STableRow< ItemPtr >::SharedThis(this))
					]
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.AutoWidth()
					[
						InArgs._Content.Widget
					]
				]
			];

		STableRow < ItemPtr >::ConstructInternal(
			typename STableRow< ItemPtr >::FArguments()
			.Style(FAppStyle::Get(), "DetailsView.TreeView.TableRow")
			.ShowSelection(false),
			InOwnerTableView
		);
	}

	const FSlateBrush* GetBackgroundImage() const
	{
		if (STableRow<ItemPtr>::IsHovered())
		{
			return FAppStyle::Get().GetBrush("Brushes.Secondary");
		}
		else
		{
			return FAppStyle::Get().GetBrush("Brushes.Header");
		}
	}

	virtual void SetContent(TSharedRef< SWidget > InContent) override
	{
		ContentBorder->SetContent(InContent);
	}

	virtual void SetRowContent(TSharedRef< SWidget > InContent) override
	{
		ContentBorder->SetContent(InContent);
	}

	virtual const FSlateBrush* GetBorder() const
	{
		return nullptr;
	}

	FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			STableRow<ItemPtr>::ToggleExpansion();
			return FReply::Handled();
		}
		else
		{
			return FReply::Unhandled();
		}
	}

	
};

class SGraphItemRow :public STableRow<ItemPtr>
{

public:

	SLATE_BEGIN_ARGS(SGraphItemRow) {}
		SLATE_DEFAULT_SLOT(typename SGraphItemRow::FArguments, Content)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		STableRow<ItemPtr>::ConstructInternal(
			STableRow<ItemPtr>::FArguments()
			.ShowSelection(true),
			InOwnerTableView
		);
	}
};

#pragma region FGraphDetailsViewItem

void FGraphDetailsViewItem::AddChild(ItemPtr Item, bool DeferRefresh)
{ 
	Item->Parent = this->AsWeak();
	this->Children.Add(Item);
	Item->SetTableView(this->TableOwner);

	if (!DeferRefresh && this->TableOwner.IsValid())
	{
		this->Sort();
		this->TableOwner.Pin()->RequestListRefresh();
	}
}

void FGraphDetailsViewItem::Sort()
{
	this->Children.Sort([&](const ItemPtr& A, const ItemPtr& B)
		{
			return A->SortKey() < B->SortKey();
		});
}

void FGraphDetailsViewItem::RemoveChild(ItemPtr Item, bool DeferRefresh)
{
	this->Children.Remove(Item);

	if (!DeferRefresh)
	{
		this->Sort();
		this->TableOwner.Pin()->RequestListRefresh();
	}
}

void FGraphDetailsViewItem::SetTableView(TableWeakPtr NewTableOwner)
{
	this->TableOwner = NewTableOwner;
}

TSharedRef<ITableRow> FGraphDetailsViewItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	// This widget is only used for debug purposes.
	TSharedPtr<IToolTip> SectionToolTip;
	TSharedPtr<STableRow<ItemPtr>> TableRow;

	TableRow = SNew(SCategoryHeaderTableRow, OwnerTable)
		.ToolTip(SectionToolTip);
	TableRow->SetRowContent(SNew(STextBlock).Text(FText::FromString("NULL WIDGET")));

	return TableRow.ToSharedRef();
}

#pragma endregion

TSharedRef<ITableRow> FHeaderItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	TSharedPtr<IToolTip> SectionToolTip;
	TSharedPtr<STableRow<ItemPtr>> TableRow;

	TableRow = SNew(SCategoryHeaderTableRow, OwnerTable)
		.ToolTip(SectionToolTip);

	TSharedPtr<SHorizontalBox> RowContainer;
	TableRow->SetRowContent
	(
		SAssignNew(RowContainer, SHorizontalBox)
	);

	TSharedPtr<SWidget> RowContent = SNew(SRichTextBlock)
		.Text(this->HeaderText)
		.TransformPolicy(ETextTransformPolicy::ToUpper)
		.DecoratorStyleSet(&FAppStyle::Get())
		.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle");

	FMargin RowPadding = FMargin(0, 2);

	TSharedPtr<SExpanderArrow> ExpanderWidget;

	ExpanderWidget =
		SNew(SExpanderArrow, TableRow)
		.BaseIndentLevel(1);

	RowContainer->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Right)
		[
			ExpanderWidget.ToSharedRef()
		];

	RowContainer->AddSlot()
		.FillWidth(1.0)
		.Padding(RowPadding)
		[
			RowContent.ToSharedRef()
		];

	return TableRow.ToSharedRef();
}

FCreateHeaderItem::FCreateHeaderItem(UEdStateGraph* Graph, FText Text)
: FHeaderItem(Text),
	GraphRef(Graph)
	
{

}

TSharedRef<ITableRow> FCreateHeaderItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	TSharedPtr<IToolTip> SectionToolTip;
	TSharedPtr<STableRow<ItemPtr>> TableRow;

	TableRow = SNew(SCategoryHeaderTableRow, OwnerTable)
		.ToolTip(SectionToolTip);

	TSharedPtr<SHorizontalBox> RowContainer;
	TableRow->SetRowContent
	(
		SAssignNew(RowContainer, SHorizontalBox)
	);

	TSharedPtr<SWidget> RowContent = SNew(SRichTextBlock)
		.Text(this->GetHeaderText())
		.TransformPolicy(ETextTransformPolicy::ToUpper)
		.DecoratorStyleSet(&FAppStyle::Get())
		.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle");

	FMargin RowPadding = FMargin(0, 0);

	TSharedPtr<SExpanderArrow> ExpanderWidget;

	ExpanderWidget =
		SNew(SExpanderArrow, TableRow)
		.BaseIndentLevel(1);

	RowContainer->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Right)
		[
			ExpanderWidget.ToSharedRef()
		];

	RowContainer->AddSlot()
		.FillWidth(1.0)
		.VAlign(VAlign_Center)
		.Padding(RowPadding)
		[
			RowContent.ToSharedRef()
		];

	RowContainer->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Right)
		.Padding(RowPadding)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.OnClicked_Raw(this, &FCreateHeaderItem::OnAddClicked)
			.Text(FText::FromString("Add"))
			.ContentPadding(FMargin(1, 0))		
			[
				SNew(SImage)
					.Image(FAppStyle::Get().GetBrush("Icons.PlusCircle"))
					.ColorAndOpacity(FSlateColor::UseForeground())
			]
		];

	return TableRow.ToSharedRef();
}

FReply FEventHeaderItem::OnAddImplementation()
{
	if (this->GraphRef.IsValid())
	{
		this->GraphRef->CreateEvent();
	}

	return FReply::Handled();
}


#pragma region StateItem

FStateItem::FStateItem(UEdStateNode* NodePtr) : NodeRef(NodePtr) 
{
	NodePtr->Events.OnNameChanged.AddRaw(this, &FStateItem::OnNameChanged);
	NodePtr->Events.OnNodeDeleted.AddRaw(this, &FStateItem::OnNodeDeleted);
}

FStateItem::~FStateItem()
{
	if (this->NodeRef.IsValid())
	{
		this->NodeRef->Events.OnNameChanged.RemoveAll(this);
		this->NodeRef->Events.OnNodeDeleted.RemoveAll(this);
	}
}

TSharedRef<ITableRow> FStateItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	auto TableRow = SNew(STableRow<ItemPtr>, OwnerTable)
		.ShowSelection(true);		

	// Make the text widget.
	TSharedPtr<SWidget> TextWidget;
	{

		SAssignNew(TextWidget, SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Right)
		[
			SNew(SExpanderArrow, TableRow)
				.BaseIndentLevel(2)
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1.0)
		.Padding(0, 2)
		[
			SAssignNew(InlineText, SInlineEditableTextBlock)
				.Text(FText::FromName(this->NodeRef->GetNodeName()))
				.OnVerifyTextChanged(this, &FStateItem::OnVerifyNameTextChanged)
				.OnTextCommitted(this, &FStateItem::OnNameTextCommited)
				.IsSelected(
					TableRow, 
					&STableRow<ItemPtr>::IsSelectedExclusively)
				.IsReadOnly_Raw(this, &FStateItem::IsReadOnly)
		];
	}
	
	TableRow->SetRowContent(TextWidget.ToSharedRef());

	return TableRow;
}

bool FStateItem::OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage)
{
	FName NewName(InText.ToString());

	if (NewName == this->NodeRef->GetNodeName()) { return true; }

	if (this->NodeRef.IsValid())
	{
		if (auto StateGraph = Cast<UEdStateGraph>(this->NodeRef->GetGraph()))
		{
			if (StateGraph->IsStateNameAvailable(NewName))
			{
				return true;
			}
			else
			{
				OutErrorMessage = FText::FromString("State Name is in use.");
			}
		}
		else
		{
			OutErrorMessage = FText::FromString("ERROR: Graph was null?");
		}
	}
	else
	{
		OutErrorMessage = FText::FromString("ERROR: Node reference was null?");
	}

	return false;
}

bool FStateItem::IsReadOnly() const
{
	if (this->NodeRef.IsValid())
	{
		if (this->NodeRef->GetNodeType() == EStateNodeType::INLINE_NODE)
		{
			return false;
		}
	}
	
	return true;
}

void FStateItem::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	if (this->NodeRef.IsValid())
	{
		FName NewName(InText.ToString());
		this->NodeRef->RenameNode(NewName);
	}
}

void FStateItem::OnNameChanged(FName Old, FName Name)
{
	if (this->InlineText.IsValid())
	{
		this->InlineText->SetText(FText::FromName(Name));
	}
}

void FStateItem::OnNodeDeleted()
{
	this->GetParent().Pin()->RemoveChild(this->AsShared());
}

void FStateItem::Delete(bool DeferRefresh)
{
	this->NodeRef->Delete();
}

#pragma endregion

#pragma region StateMachineItem

FStateMachineItem::FStateMachineItem(UEdStateGraph* GraphPtr)
: GraphRef(GraphPtr)
{
	GraphPtr->Events.OnNameChanged.AddRaw(this, &FStateMachineItem::OnNameChanged);
	GraphPtr->Events.OnGraphDeleted.AddRaw(this, &FStateMachineItem::OnGraphDeleted);
	//GraphPtr->Events.OnEventCreated.AddRaw(this, &FStateMachineItem::OnEventCreated, false);
}

FStateMachineItem::~FStateMachineItem()
{
	if (this->GraphRef.IsValid())
	{
		//this->GraphRef->Events.OnNameChanged.RemoveAll(this);
		this->GraphRef->Events.OnGraphDeleted.RemoveAll(this);
		//this->GraphRef->Events.OnEventCreated.RemoveAll(this);
	}
}

void FStateMachineItem::Select()
{
	this->GraphRef->Select();
}

FString FStateMachineItem::SortKey() const
{
	if (this->GraphRef.IsValid())
	{
		if (GraphRef->IsMainGraph())
		{
			return "Main";
		}
		else
		{
			return GraphRef->GetName();
		}
	}
	else
	{
		return "";
	}
}

TSharedRef<ITableRow> FStateMachineItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool _bIsReadOnly)
{
	auto TableRow = SNew(STableRow<ItemPtr>, OwnerTable)
		.ShowSelection(true);
	
	// Make the text widget.
	TSharedPtr<SWidget> TextWidget;
	{

		SAssignNew(TextWidget, SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.FillWidth(1.0)
			.Padding(2, 2)
			[
				SAssignNew(InlineText, SInlineEditableTextBlock)
					//.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
					.Text(FText::FromName(this->GraphRef->GetFName()))
					.OnVerifyTextChanged(this, &FStateMachineItem::OnVerifyNameTextChanged)
					.OnTextCommitted(this, &FStateMachineItem::OnNameTextCommited)
					.IsSelected(
						TableRow,
						&STableRow<ItemPtr>::IsSelectedExclusively)
					//.IsReadOnly(false)
					//.IsReadOnly(&FStateMachineItem::IsReadOnly)
					//.IsReadOnly_Lambda([this]() { return this->IsReadOnly(); })
					.IsReadOnly_Raw(this, &FStateMachineItem::IsReadOnly)
			];
	}

	TableRow->SetRowContent(TextWidget.ToSharedRef());

	return TableRow;
}

void FStateMachineItem::OnEventCreated(UEdEventObject* EventObject, bool DeferRefresh)
{
	/*this->EventRoot->AddChild(
		MakeShareable(new FEventItem(EventObject)), DeferRefresh);*/
}

bool FStateMachineItem::IsReadOnly() const
{
	if (this->GraphRef.Get()->IsMainGraph())
	{
		return true;
	}
	else if (this->GraphRef.Get()->GetGraphType() != EStateMachineGraphType::SUB_GRAPH)
	{
		switch (this->GraphRef.Get()->GetGraphType())
		{
			case EStateMachineGraphType::SUB_GRAPH: return false;
			case EStateMachineGraphType::MAIN_GRAPH: return true;
			case EStateMachineGraphType::EXTENDED_GRAPH: return true;
		}
	}

	return false;
}

void FStateMachineItem::OnGraphChanged(const FEdGraphEditAction& Action)
{
	if (Action.Action & EEdGraphActionType::GRAPHACTION_AddNode)
	{
		for (auto& Node : Action.Nodes)
		{
			if (auto StateNode = Cast<UEdStateNode>(Node))
			{
				this->AddState(const_cast<UEdStateNode*>(StateNode), false);
			}
			else if (auto EventNode = Cast<UEdTransition>(Node))
			{
				//this->AddTransition(const_cast<UEdTransition*>(EventNode));
			}
		}
	}
}

void FStateMachineItem::AddState(UEdStateNode* State, bool DeferRefresh)
{

}

bool FStateMachineItem::OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage)
{
	FName NewName(InText.ToString());

	if (NewName == this->GraphRef->GetFName()) { return true; }

	if (this->GraphRef.IsValid())
	{
		if (auto BP = this->GraphRef->GetBlueprintOwner())
		{
			BP->RenameGraph(this->GraphRef.Get(), NewName);
			return true;
		}
		else
		{
			OutErrorMessage = FText::FromString("ERROR: Blueprint Owner was null?");
		}

	}
	else
	{
		OutErrorMessage = FText::FromString("ERROR: Graph reference was null?");
	}

	return false;
}

void FStateMachineItem::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	FName NewName(InText.ToString());
	this->GraphRef->RenameGraph(NewName);
}

void FStateMachineItem::OnNameChanged(FName OldName, FName Name)
{
	this->InlineText->SetText(FText::FromName(Name));
}

void FStateMachineItem::OnGraphDeleted()
{
	this->GetParent().Pin()->RemoveChild(this->AsShared());
}

void FStateMachineItem::Delete(bool DeferRefresh)
{
	if (this->GraphRef.IsValid())
	{
		this->GraphRef->Delete();
	}
}

#pragma endregion

#pragma region TransitionItem

FTransitionItem::FTransitionItem(UEdTransition* NodePtr) : NodeRef(NodePtr) 
{
	NodePtr->Events.OnNodeDeleted.AddRaw(this, &FTransitionItem::OnNodeDeleted);
}

FTransitionItem::~FTransitionItem()
{

}

TSharedRef<ITableRow> FTransitionItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	auto TableRow = SNew(STableRow<ItemPtr>, OwnerTable)
		.ShowSelection(true);

	// Make the text widget.
	TSharedPtr<SWidget> TextWidget;
	{
		FName FromName = this->NodeRef->GetStartNode()->GetNodeName();
		FName ToName = this->NodeRef->GetEndNode()->GetNodeName();

		SAssignNew(TextWidget, SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Right)
			[
				SNew(SExpanderArrow, TableRow)
					.BaseIndentLevel(2)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0)
			.Padding(0, 2)
			[
				SNew(STextBlock)
					.Text(FText::FromName(FromName))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0)
			.Padding(0, 2)
			[
				SNew(STextBlock)
					.Text(FText::FromString("->"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0)
			.Padding(0, 2)
			[
				SNew(STextBlock)
					.Text(FText::FromName(ToName))
			];
	}

	TableRow->SetRowContent(TextWidget.ToSharedRef());

	return TableRow;
}


void FTransitionItem::OnNodeDeleted()
{
	this->GetParent().Pin()->RemoveChild(this->AsShared());
}

#pragma endregion

#pragma region Eventitem

FEventItem::FEventItem(TWeakObjectPtr<UEdEventObject> EventObject)
	: EventReference(EventObject)
{
	EventObject->Events.OnEventRenamed.AddRaw(this, &FEventItem::OnEventRenamed);
	EventObject->Events.OnEventRemoved.AddRaw(this, &FEventItem::OnEventDeleted);
}

TSharedRef<ITableRow> FEventItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	//auto TableRow = SNew(STableRow<ItemPtr>, OwnerTable)
	auto TableRow = SNew(SGraphItemRow, OwnerTable);

	// Make the text widget.
	TSharedPtr<SWidget> TextWidget;
	{

		SAssignNew(TextWidget, SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Right)
			[
				SNew(SExpanderArrow, TableRow)
					.BaseIndentLevel(2)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0)
			.Padding(0, 2)
			[
				SAssignNew(InlineText, SInlineEditableTextBlock)
				//.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
				.Text(FText::FromName(this->EventReference.Get()->GetEventName()))
				.OnVerifyTextChanged(this, &FEventItem::OnVerifyNameTextChanged)
				.OnTextCommitted(this, &FEventItem::OnNameTextCommited)
				.IsSelected(
					TableRow,
					&STableRow<ItemPtr>::IsSelectedExclusively)
			];
	}

	TableRow->SetRowContent(TextWidget.ToSharedRef());

	return TableRow;
}

void FEventItem::Select()
{
	this->EventReference->Inspect(); 
}

void FEventItem::OnEventRenamed(FName From, FName To)
{
	this->InlineText->SetText(FText::FromName(To));
}

void FEventItem::OnEventDeleted()
{
	this->GetParent().Pin()->RemoveChild(this->AsShared());
}

bool FEventItem::OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage)
{
	FName NewName(InText.ToString());

	if (this->EventReference.IsValid())
	{
		if (NewName == this->EventReference.Get()->GetEventName()) { return true; }

		if (this->EventReference.Get()->GetGraph()->IsEventNameAvailable(NewName))
		{
			return true;
		}
		else
		{
			OutErrorMessage = FText::FromString("Event Name is in use.");
		}
	}
	else
	{
		OutErrorMessage = FText::FromString("ERROR: Node reference was null?");
	}

	return false;
}

void FEventItem::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	FName NewName(InText.ToString());
	this->EventReference.Get()->RenameEvent(NewName);
}

void FEventItem::Delete(bool DeferRefresh)
{
	this->EventReference->Delete();
}

#pragma endregion

void SSubGraphDetails::Construct(const FArguments& InArgs, UEdStateGraph* GraphPtr)
{
	this->GraphRef = GraphPtr;
	this->StateRoot = MakeShareable(new FHeaderItem("States"));
	this->EventRoot = MakeShareable(new FEventHeaderItem(GraphPtr, "Events"));

	this->TreeViewList.Add(this->StateRoot);
	this->TreeViewList.Add(this->EventRoot);


	SAssignNew(TreeView, STreeView<ItemPtr>)
		.ItemHeight(24)
		.TreeItemsSource(&this->TreeViewList)
		.OnGenerateRow(this, &SSubGraphDetails::OnGenerateRow, false)
		.SelectionMode(ESelectionMode::Single)
		.OnSelectionChanged(this, &SSubGraphDetails::OnSelectionChanged)
		.OnMouseButtonDoubleClick(this, &SSubGraphDetails::OnItemDoubleClicked)
		//.OnContextMenuOpening(InArgs._OnContextMenuOpening)
		.OnGetChildren(this, &SSubGraphDetails::OnGetChildrenForCategory)
		.OnItemScrolledIntoView(this, &SSubGraphDetails::OnItemScrolledIntoView)
		.OnSetExpansionRecursive(this, &SSubGraphDetails::OnSetExpansionRecursive)
		.HighlightParentNodesForSelection(true);

	this->StateRoot->SetTableView(this->TreeView);
	this->EventRoot->SetTableView(this->TreeView);

	ChildSlot
	[
		SNew(SScrollBorder, this->TreeView.ToSharedRef())
		[
			this->TreeView.ToSharedRef()
		]
	];

	this->TreeView->SetItemExpansion(this->StateRoot, true);
	this->TreeView->SetItemExpansion(this->EventRoot, true);

	this->BindEvents();
}

void SSubGraphDetails::BindEvents()
{
	this->GraphRef->Events.OnStateAdded.AddSP(this, &SSubGraphDetails::OnStateAdded);
	this->GraphRef->Events.OnEventCreated.AddSP(this, &SSubGraphDetails::OnEventAdded);
	this->GraphRef->Events.OnGraphDataReverted.AddSP(this, &SSubGraphDetails::OnGraphDataReverted);
}

void SSubGraphDetails::OnGraphDataReverted()
{
	this->Refresh();
}

void SSubGraphDetails::OnStateAdded(UEdStateNode* State)
{
	this->AddState(State, false);
}

void SSubGraphDetails::OnEventAdded(UEdEventObject* Event)
{
	this->AddEvent(Event, false);
}

void SSubGraphDetails::AddState(UEdStateNode* Node, bool DeferRefresh)
{
	ItemPtr Item = MakeShareable(new FStateItem(Node));

	this->StateRoot->AddChild(Item, DeferRefresh);
}

void SSubGraphDetails::AddEvent(UEdEventObject* Node, bool DeferRefresh)
{
	ItemPtr Item = MakeShareable(new FEventItem(Node));

	this->EventRoot->AddChild(Item, DeferRefresh);
}

FReply SSubGraphDetails::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	auto Selected = this->TreeView->GetSelectedItems();
	if (KeyEvent.GetKey() == EKeys::Delete || KeyEvent.GetKey() == EKeys::BackSpace)
	{
		for (auto& Item : Selected)
		{
			Item->Delete(true);
		}

		return FReply::Handled();
	}
	else if (KeyEvent.GetKey() == EKeys::F2 && Selected.Num() == 1)
	{
		auto& Item = Selected[0];
		this->TreeView->RequestScrollIntoView(Item);
		Item->EnterRenameMode();

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void SSubGraphDetails::OnGetChildrenForCategory(
	ItemPtr InItem,
	TArray< ItemPtr >& OutChildren)
{
	InItem->GetChildren(OutChildren);
}

TSharedRef<ITableRow> SSubGraphDetails::OnGenerateRow(
	ItemPtr InItem,
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	return InItem->GetEntryWidget(OwnerTable, bIsReadOnly);
}

void SSubGraphDetails::OnSelectionChanged(
	ItemPtr SelectedItem,
	ESelectInfo::Type SelectInfo)
{
	if (SelectedItem.IsValid())
	{
		SelectedItem->Select();
	}
}

void SSubGraphDetails::AddReferencedObjects(FReferenceCollector& Collector)
{

}

void SSubGraphDetails::Refresh()
{
	this->StateRoot->Clear();
	this->EventRoot->Clear();

	for (auto& Node : GraphRef->GetStates())
	{
		this->AddState(Node, true);
	}

	for (auto& Ev : GraphRef->GetEventList())
	{
		this->AddEvent(Ev, true);
	}

	this->StateRoot->Sort();
	this->EventRoot->Sort();
		
	this->TreeView->RequestListRefresh();
}

void SGraphDetailsView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InEditor) 
{
	this->EditorPtr = InEditor;

	this->TreeViewRoot = MakeShareable(new FGraphDetailsViewItem);

	SAssignNew(TreeView, STreeView<ItemPtr>)
		.ItemHeight(24)
		.TreeItemsSource(&this->TreeViewRoot->ReadChildren())
		.OnGenerateRow(this, &SGraphDetailsView::OnGenerateRow, false)
		.SelectionMode(ESelectionMode::Single)
		.OnSelectionChanged(this, &SGraphDetailsView::OnSelectionChanged)
		.OnMouseButtonDoubleClick(this, &SGraphDetailsView::OnItemDoubleClicked)
		//.OnContextMenuOpening(InArgs._OnContextMenuOpening)
		.OnGetChildren(this, &SGraphDetailsView::OnGetChildrenForCategory)
		.OnItemScrolledIntoView(this, &SGraphDetailsView::OnItemScrolledIntoView)
		.OnSetExpansionRecursive(this, &SGraphDetailsView::OnSetExpansionRecursive)
		.HighlightParentNodesForSelection(true);

	// Set up the header for State Machines list.
	TSharedPtr<SHorizontalBox> RowContainer;
	{
		SAssignNew(RowContainer, SHorizontalBox);

		TSharedPtr<SWidget> RowContent = SNew(SRichTextBlock)
			.Text(FText::FromString("State Machines"))
			.TransformPolicy(ETextTransformPolicy::ToUpper)
			.DecoratorStyleSet(&FAppStyle::Get())
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle");

		FMargin RowPadding = FMargin(0, 0);

		RowContainer->AddSlot()
			.FillWidth(1.0)
			.VAlign(VAlign_Center)
			.Padding(RowPadding)
			[
				RowContent.ToSharedRef()
			];

		RowContainer->AddSlot()
			.AutoWidth()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Right)
			.Padding(RowPadding)
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.OnClicked_Raw(this, &SGraphDetailsView::OnAddStateMachine)
				.Text(FText::FromString("Add"))
				.ContentPadding(FMargin(1, 0))
				[
					SNew(SImage)
						.Image(FAppStyle::Get().GetBrush("Icons.PlusCircle"))
						.ColorAndOpacity(FSlateColor::UseForeground())
				]
			];
	}

	ChildSlot
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(FilterBox, SSearchBox)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			RowContainer.ToSharedRef()
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBorder, this->TreeView.ToSharedRef())
			[
				this->TreeView.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SAssignNew(DetailsTabs, SWidgetSwitcher)
		]
	];

	//this->TreeView->SetItemExpansion(this->TreeListRoot, true);
	this->TreeViewRoot->SetTableView(this->TreeView);

	this->BindEvents(InEditor);
	this->InitView(InEditor);
}

void SGraphDetailsView::InitView(TSharedPtr<FEditor> InEditor)
{
	auto BP = InEditor->GetStateMachineBlueprintObj();
	this->InitGraphDetailsView(BP->GetMainGraph(), true);

	for (auto SubGraph : BP->GetSubgraphs())
	{
		this->InitGraphDetailsView(SubGraph, true);
	}
	
	this->TreeViewRoot->Sort();
	this->DetailsTabs->SetActiveWidgetIndex(0);
	this->TreeView->RequestListRefresh();
}

void SGraphDetailsView::InitGraphDetailsView(UEdStateGraph* Graph, bool bDeferRefresh)
{
	ItemPtr Item = MakeShareable(new FStateMachineItem(Graph));
	auto Widget = SNew(SSubGraphDetails, Graph);

	this->TreeViewRoot->AddChild(Item, bDeferRefresh);
	Item->SetTableView(this->TreeView);
	this->TreeView->SetItemExpansion(Item, true);
	this->DetailsTabs->AddSlot(this->DetailsTabs->GetNumWidgets())[Widget];
	this->GraphToWidgetMap.Add(Graph, Widget);

	Widget->Refresh();
}

void SGraphDetailsView::OnGraphSelected(UEdStateGraph* Graph)
{
	if (this->GraphToWidgetMap.Contains(Graph))
	{
		this->DetailsTabs->SetActiveWidget(this->GraphToWidgetMap[Graph].ToSharedRef());
	}
}

FReply SGraphDetailsView::OnAddStateMachine()
{
	auto BP = this->EditorPtr.Pin()->GetStateMachineBlueprintObj();
	auto NewGraph = BP->AddSubGraph();
	this->InitGraphDetailsView(NewGraph, false);

	return FReply::Handled();
}

void SGraphDetailsView::OnStateAdded(UEdStateNode* Node)
{
	auto Graph = Cast<UEdStateGraph>(Node->GetGraph());

	if (this->GraphToWidgetMap.Contains(Graph))
	{
		this->GraphToWidgetMap[Graph]->AddState(Node, false);
	}
}

void SGraphDetailsView::OnSelectionChanged(
	ItemPtr SelectedItem,
	ESelectInfo::Type SelectInfo)
{
	if (SelectedItem.IsValid())
	{ 
		SelectedItem->Select();
	}	
}

TSharedRef<ITableRow> SGraphDetailsView::OnGenerateRow(
	ItemPtr InItem,
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	return InItem->GetEntryWidget(OwnerTable, bIsReadOnly);
}

void SGraphDetailsView::AddReferencedObjects(FReferenceCollector& Collector) {

}

void SGraphDetailsView::BindEvents(TSharedPtr<class FEditor> InEditor) {
	if (auto BPObj = InEditor->GetStateMachineBlueprintObj())
	{
		auto Graph = BPObj->GetMainGraph();
		check(Graph);

		BPObj->Events.OnGraphSelected.AddSP(this, &SGraphDetailsView::OnGraphSelected);
	}
}

void SGraphDetailsView::OnGetChildrenForCategory(
	ItemPtr InItem, 
	TArray< ItemPtr >& OutChildren)
{
	InItem->GetChildren(OutChildren);
}

void FGraphDetailsViewItem::GetChildren(TArray< ItemPtr >& OutChildren) const
{
	for (auto& Child : this->Children)
	{
		OutChildren.Add(Child);
	}
}

FReply SGraphDetailsView::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	auto Selected = this->TreeView->GetSelectedItems();

	if (KeyEvent.GetKey() == EKeys::Delete || KeyEvent.GetKey() == EKeys::BackSpace)
	{
		for (auto& Item : Selected)
		{
			Item->Delete(true);
		}

		return FReply::Handled();
	}
	else if (KeyEvent.GetKey() == EKeys::F2 && Selected.Num() == 1)
	{
		auto& Item = Selected[0];
		this->TreeView->RequestScrollIntoView(Item);
		Item->EnterRenameMode();

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE