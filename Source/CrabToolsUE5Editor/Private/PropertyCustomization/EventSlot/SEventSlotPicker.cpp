#include "PropertyCustomization/EventSlot/SEventSlotPicker.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Images/SImage.h"
#include "Styling/AppStyle.h"
#include "Widgets/SWindow.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/ARFilter.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Textures/SlateIcon.h"
#include "PropertyHandle.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/DataStructures.h"
#include "Widgets/Input/SSearchBox.h"
#include "Framework/Application/SlateApplication.h"
#include "AssetRegistry/AssetData.h"
#include "Editor.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#define LOCTEXT_NAMESPACE "EventSlotPicker"


SEventSlotPicker::~SEventSlotPicker()
{

}

void SEventSlotPicker::Construct(const FArguments& InArgs)
{
	this->SetCanTick(false);
	this->Root = MakeShareable(new FRootEventSlotNode());

	OnTagChanged = InArgs._OnTagChanged;
	OnRefreshTagContainers = InArgs._OnRefreshTagContainers;
	bReadOnly = InArgs._ReadOnly;
	SettingsName = InArgs._SettingsName;
	PropertyHandle = InArgs._PropertyHandle;
	FilterString = InArgs._Filter;
	MaxHeight = InArgs._MaxHeight;
	

	PostUndoRedoDelegateHandle = FEditorDelegates::PostUndoRedo.AddSP(this, &SEventSlotPicker::OnPostUndoRedo);

	TSharedPtr<SComboButton> SettingsCombo = SNew(SComboButton)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.ButtonStyle(FAppStyle::Get(), "SimpleButton")
		.HasDownArrow(false)
		.ButtonContent()
		[
			SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Settings"))
				.ColorAndOpacity(FSlateColor::UseForeground())
		];
	SettingsCombo->SetOnGetMenuContent(FOnGetContent::CreateSP(this, &SEventSlotPicker::MakeSettingsMenu, SettingsCombo));


	TWeakPtr<SEventSlotPicker> WeakSelf = SharedThis(this);

	TSharedRef<SWidget> Picker =
		SNew(SBorder)
		.Padding(InArgs._Padding)
		.BorderImage(FStyleDefaults::GetNoBrush())
		[
			SNew(SVerticalBox)

				// Gameplay Tag Tree controls
				+ SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Top)
				[
					SNew(SHorizontalBox)
						// Search
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.FillWidth(1.f)
						.Padding(0, 1, 5, 1)
						[
							SAssignNew(SearchTagBox, SSearchBox)
								.HintText(LOCTEXT("GameplayTagPicker_SearchBoxHint", "Search Gameplay Tags"))
								.OnTextChanged(this, &SEventSlotPicker::OnFilterTextChanged)
						]

						// View settings
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SettingsCombo.ToSharedRef()
						]
				]

				// Event Slot tree
				+ SVerticalBox::Slot()
				.MaxHeight(MaxHeight)
				.FillHeight(1)
				[
					SAssignNew(TagTreeContainerWidget, SBorder)
						.BorderImage(FStyleDefaults::GetNoBrush())
						[
							SAssignNew(TagTreeWidget, STreeView<TSharedPtr<FEventSlotNode>>)
								.TreeItemsSource(&this->Root->GetFilteredChildNodes(this->FilterString))
								.OnGenerateRow(this, &SEventSlotPicker::OnGenerateRow)
								.OnGetChildren(this, &SEventSlotPicker::OnGetChildren)
								.OnExpansionChanged(this, &SEventSlotPicker::OnExpansionChanged)
								.SelectionMode(ESelectionMode::Single)
								.OnContextMenuOpening(this, &SEventSlotPicker::OnTreeContextMenuOpening)
								.OnSelectionChanged(this, &SEventSlotPicker::OnTreeSelectionChanged)
								.OnKeyDownHandler(this, &SEventSlotPicker::OnTreeKeyDown)
						]
				]
		];

	

	if (InArgs._ShowMenuItems)
	{
		FMenuBuilder MenuBuilder(false, nullptr);

		MenuBuilder.BeginSection(FName(), LOCTEXT("SectionGameplayTag", "GameplayTag"));

		MenuBuilder.AddMenuEntry(
			LOCTEXT("GameplayTagPicker_ClearSelection", "Clear Selection"), FText::GetEmpty(), FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.X"),
			FUIAction(FExecuteAction::CreateRaw(this, &SEventSlotPicker::OnMenuClearClicked, TSharedPtr<SComboButton>()))
		);

		MenuBuilder.AddSeparator();

		TSharedRef<SWidget> MenuContent =
			SNew(SBox)
			.WidthOverride(300.0f)
			.HeightOverride(MaxHeight)
			[
				Picker
			];
		MenuBuilder.AddWidget(MenuContent, FText::GetEmpty(), true);

		MenuBuilder.EndSection();

		ChildSlot
			[
				MenuBuilder.MakeWidget()
			];
	}
	else
	{
		ChildSlot
			[
				Picker
			];
	}

	// Force the entire tree collapsed to start
	SetTagTreeItemExpansion(false, false);

	this->TagTreeWidget->RequestTreeRefresh();
	this->InitRoot();
}

void SEventSlotPicker::InitRoot()
{
	this->Root->OnSelectionUpdated.AddRaw(this, &SEventSlotPicker::OnEventSelectionChanged);
}

void SEventSlotPicker::OnEventSelectionChanged()
{
	this->SetEventName(this->Root->GetEventName());
}

void SEventSlotPicker::SetEventName(FName EName)
{
	if (auto StructProp = CastField<FStructProperty>(this->PropertyHandle->GetProperty()))
	{
		if (StructProp->Struct == FEventSlot::StaticStruct())
		{
			PropertyHandle->SetValueFromFormattedString(
				FString::Printf(TEXT("(EventName=\"%s\")"), *EName.ToString()));
		}
	}
}

TSharedPtr<SWidget> SEventSlotPicker::OnTreeContextMenuOpening()
{
	TArray<TSharedPtr<FEventSlotNode>> Selection = TagTreeWidget->GetSelectedItems();
	const TSharedPtr<FEventSlotNode> SelectedTagNode = Selection.IsEmpty() ? nullptr : Selection[0];
	return MakeEventActionsMenu(SelectedTagNode, TSharedPtr<SComboButton>(), true);
}

void SEventSlotPicker::OnTreeSelectionChanged(TSharedPtr<FEventSlotNode> SelectedItem, ESelectInfo::Type SelectInfo)
{
	
}

FReply SEventSlotPicker::OnTreeKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (FSlateApplication::Get().GetNavigationActionFromKey(InKeyEvent) == EUINavigationAction::Accept)
	{
		TArray<TSharedPtr<FEventSlotNode>> Selection = TagTreeWidget->GetSelectedItems();
		TSharedPtr<FEventSlotNode> SelectedItem;
		if (!Selection.IsEmpty())
		{
			SelectedItem = Selection[0];
		}

		if (SelectedItem.IsValid())
		{
			TGuardValue<bool> PersistExpansionChangeGuard(bInSelectionChanged, true);

			// Toggle selection
			const ECheckBoxState State = IsTagChecked(SelectedItem);
			if (State == ECheckBoxState::Unchecked)
			{
				//OnTagChecked(SelectedItem);
			}
			else if (State == ECheckBoxState::Checked)
			{
				//OnTagUnchecked(SelectedItem);
			}

			return FReply::Handled();
		}
	}

	return SCompoundWidget::OnKeyDown(InGeometry, InKeyEvent);
}

TSharedRef<SWidget> SEventSlotPicker::MakeSettingsMenu(TSharedPtr<SComboButton> OwnerCombo)
{
	FMenuBuilder MenuBuilder(false, nullptr);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("GameplayTagPicker_ExpandAll", "Expand All"), FText::GetEmpty(), FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &SEventSlotPicker::OnExpandAllClicked, OwnerCombo))
	);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("GameplayTagPicker_CollapseAll", "Collapse All"), FText::GetEmpty(), FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &SEventSlotPicker::OnCollapseAllClicked, OwnerCombo))
	);

	return MenuBuilder.MakeWidget();
}

void SEventSlotPicker::OnFilterTextChanged(const FText& InFilterText)
{
	FilterString = InFilterText.ToString();
	FilterTagTree();
}

void SEventSlotPicker::FilterTagTree()
{
	TagTreeWidget->SetTreeItemsSource(&this->Root->GetFilteredChildNodes(this->FilterString));
	TagTreeWidget->RequestTreeRefresh();
}

FText SEventSlotPicker::GetHighlightText() const
{
	return FilterString.IsEmpty() ? FText::GetEmpty() : FText::FromString(FilterString);
}

TSharedRef<ITableRow> SEventSlotPicker::OnGenerateRow(TSharedPtr<FEventSlotNode> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	FText TooltipText;
	FString TagSource;
	bool bIsExplicitTag = true;

	TSharedPtr<SComboButton> ActionsCombo = SNew(SComboButton)
		.ToolTipText(LOCTEXT("MoreActions", "More Actions..."))
		.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
		.ContentPadding(0)
		.ForegroundColor(FSlateColor::UseForeground())
		.HasDownArrow(true)
		.CollapseMenuOnParentFocus(true);

	// Craete context menu with bInShouldCloseWindowAfterMenuSelection = false, or else the actions menu action will not work due the popup-menu handling order.
	ActionsCombo->SetOnGetMenuContent(FOnGetContent::CreateSP(this, &SEventSlotPicker::MakeEventActionsMenu, InItem, ActionsCombo, /*bInShouldCloseWindowAfterMenuSelection*/false));
	return SNew(STableRow<TSharedPtr<FEventSlotNode>>, OwnerTable)
		.Style(FAppStyle::Get(), "GameplayTagTreeView")
		.ToolTipText(TooltipText)
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(SCheckBox)
						.OnCheckStateChanged(this, &SEventSlotPicker::OnNodeCheckStatusChanged, InItem)
						.IsChecked(this, &SEventSlotPicker::IsTagChecked, InItem)
						.IsEnabled(InItem->CanBeSelected())
						.CheckBoxContentUsesAutoWidth(true)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(FMargin(2, 0, 4, 0))
				[
					SNew(STextBlock)
						.HighlightText(this, &SEventSlotPicker::GetHighlightText)
						.Text(FText::FromName(InItem->GetNodeName()))
						.ColorAndOpacity(FLinearColor(1, 1, 1))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				[
					ActionsCombo.ToSharedRef()
				]
		];
}

void SEventSlotPicker::OnGetChildren(TSharedPtr<FEventSlotNode> InItem, TArray<TSharedPtr<FEventSlotNode>>& OutChildren)
{
	OutChildren.Append(InItem->GetFilteredChildNodes(this->FilterString));
}

void SEventSlotPicker::OnNodeCheckStatusChanged(ECheckBoxState NewCheckState, TSharedPtr<FEventSlotNode> NodeChanged)
{
	if (NewCheckState == ECheckBoxState::Checked)
	{
		NodeChanged->SelectEvent(true);
	}
	else if (NewCheckState == ECheckBoxState::Unchecked)
	{
		NodeChanged->SelectEvent(false);
	}
}
ECheckBoxState SEventSlotPicker::IsTagChecked(TSharedPtr<FEventSlotNode> Node) const
{
	return Node->IsSelected() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}


EVisibility SEventSlotPicker::DetermineAllowChildrenVisible(TSharedPtr<FEventSlotNode> Node) const
{
	return EVisibility::Visible;
}

void SEventSlotPicker::OnMenuClearClicked(TSharedPtr<SComboButton> OwnerCombo)
{
	this->SetEventName(NAME_None);
}

FSlateColor SEventSlotPicker::GetTagTextColour(TSharedPtr<FEventSlotNode> Node) const
{
	static const FLinearColor DefaultTextColour = FLinearColor::White;
	static const FLinearColor DescendantConflictTextColour = FLinearColor(1.f, 0.65f, 0.f); // orange
	static const FLinearColor NodeConflictTextColour = FLinearColor::Red;
	static const FLinearColor AncestorConflictTextColour = FLinearColor(1.f, 1.f, 1.f, 0.5f);

	return DefaultTextColour;
}

void SEventSlotPicker::OnExpandAllClicked(TSharedPtr<SComboButton> OwnerCombo)
{
	SetTagTreeItemExpansion(true, true);

	if (OwnerCombo.IsValid())
	{
		OwnerCombo->SetIsOpen(false);
	}
}

void SEventSlotPicker::OnCollapseAllClicked(TSharedPtr<SComboButton> OwnerCombo)
{
	SetTagTreeItemExpansion(false, true);
	if (OwnerCombo.IsValid())
	{
		OwnerCombo->SetIsOpen(false);
	}
}

TSharedRef<SWidget> SEventSlotPicker::MakeEventActionsMenu(TSharedPtr<FEventSlotNode> InTagNode, TSharedPtr<SComboButton> ActionsCombo, bool bInShouldCloseWindowAfterMenuSelection)
{
	if (!InTagNode.IsValid())
	{
		return SNullWidget::NullWidget;
	}

	// Do not close menu after selection. The close deletes this widget before action is executed leading to no action being performed.
	// Occurs when SEventSlotPicker is being used as a menu item itself (Details panel of blueprint editor for example).
	FMenuBuilder MenuBuilder(bInShouldCloseWindowAfterMenuSelection, nullptr);

	// Add child tag

	// Search for References
	if (FEditorDelegates::OnOpenReferenceViewer.IsBound())
	{
		MenuBuilder.AddMenuEntry(LOCTEXT("GameplayTagPicker_SearchForReferences", "Search For References"),
			//FText::Format(LOCTEXT("GameplayTagPicker_SearchForReferencesTooltip", "Find references to the tag {0}"), FText::AsCultureInvariant(InTagNode->GetCompleteTagString())),
			FText::Format(LOCTEXT("GameplayTagPicker_SearchForReferencesTooltip", "Find references to the tag {0}"), FText::AsCultureInvariant("??? - 1")),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Search"),
			FUIAction(FExecuteAction::CreateSP(this, &SEventSlotPicker::OnSearchForReferences, InTagNode, ActionsCombo)));
	}

	// Search for References of all children
	if (FEditorDelegates::OnOpenReferenceViewer.IsBound())
	{
		MenuBuilder.AddMenuEntry(LOCTEXT("GameplayTagPicker_SearchForReferencesAsChildren", "Search For References (all children)"),
			//FText::Format(LOCTEXT("GameplayTagPicker_SearchForReferencesAsChildrenTooltip", "Find references to the tag {0} and all its children"), FText::AsCultureInvariant(InTagNode->GetCompleteTagString())),
			FText::Format(LOCTEXT("GameplayTagPicker_SearchForReferencesTooltip", "Find references to the tag {0}"), FText::AsCultureInvariant("??? - 2")),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Search"),
			FUIAction(FExecuteAction::CreateSP(this, &SEventSlotPicker::OnSearchForReferencesAsChildren, InTagNode, ActionsCombo)));
	}

	// Copy Name to Clipboard
	MenuBuilder.AddMenuEntry(LOCTEXT("GameplayTagPicker_CopyNameToClipboard", "Copy Name to Clipboard"),
		//FText::Format(LOCTEXT("GameplayTagPicker_CopyNameToClipboardTooltip", "Copy tag {0} to clipboard"), FText::AsCultureInvariant(InTagNode->GetCompleteTagString())),
		FText::Format(LOCTEXT("GameplayTagPicker_SearchForReferencesTooltip", "Find references to the tag {0}"), FText::AsCultureInvariant("??? - 3")),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericCommands.Copy"),
		FUIAction(FExecuteAction::CreateSP(this, &SEventSlotPicker::OnCopyTagNameToClipboard, InTagNode, ActionsCombo)));

	return MenuBuilder.MakeWidget();
}

void SEventSlotPicker::OnSearchForReferences(TSharedPtr<FEventSlotNode> InTagNode, TSharedPtr<SComboButton> OwnerCombo)
{

}

void SEventSlotPicker::OnSearchForReferencesAsChildren(TSharedPtr<FEventSlotNode> InTagNode, TSharedPtr<SComboButton> OwnerCombo)
{
	
}

void SEventSlotPicker::OnCopyTagNameToClipboard(TSharedPtr<FEventSlotNode> InTagNode, TSharedPtr<SComboButton> OwnerCombo)
{
	if (InTagNode.IsValid())
	{
		FPlatformApplicationMisc::ClipboardCopy(*this->Root->GetEventName().ToString());
	}

	if (OwnerCombo.IsValid())
	{
		OwnerCombo->SetIsOpen(false);
	}
}

void SEventSlotPicker::SetTagTreeItemExpansion(bool bExpand, bool bPersistExpansion)
{
	TArray<TSharedPtr<FEventSlotNode>> TagArray;
	//UGameplayTagsManager::Get().GetFilteredGameplayRootTags(TEXT(""), TagArray);
	for (int32 TagIdx = 0; TagIdx < TagArray.Num(); ++TagIdx)
	{
		SetTagNodeItemExpansion(TagArray[TagIdx], bExpand, bPersistExpansion);
	}
}

void SEventSlotPicker::SetTagNodeItemExpansion(TSharedPtr<FEventSlotNode> Node, bool bExpand, bool bPersistExpansion)
{
	TGuardValue<bool> PersistExpansionChangeGuard(bPersistExpansionChange, bPersistExpansion);
	if (Node.IsValid() && TagTreeWidget.IsValid())
	{
		TagTreeWidget->SetItemExpansion(Node, bExpand);

		const TArray<TSharedPtr<FEventSlotNode>>& ChildTags = Node->GetChildNodes();
		for (int32 ChildIdx = 0; ChildIdx < ChildTags.Num(); ++ChildIdx)
		{
			SetTagNodeItemExpansion(ChildTags[ChildIdx], bExpand, bPersistExpansion);
		}
	}
}

void SEventSlotPicker::SetDefaultTagNodeItemExpansion(TSharedPtr<FEventSlotNode> Node)
{

}


void SEventSlotPicker::OnExpansionChanged(TSharedPtr<FEventSlotNode> InItem, bool bIsExpanded)
{

}

void SEventSlotPicker::RequestScrollToView(const FEventSlot RequestedSlot)
{

}


TSharedPtr<SWidget> SEventSlotPicker::GetWidgetToFocusOnOpen()
{
	return SearchTagBox;
}

void SEventSlotPicker::OnPostUndoRedo()
{
	
}

#pragma region Event Slot Tree

const TArray<TSharedPtr<FEventSlotNode>>& FEventSlotNode::GetChildNodes()
{
	if (!this->bHasInit)
	{
		Init();
		this->Children.Sort(
			[](const TSharedPtr<FEventSlotNode>& A, const TSharedPtr<FEventSlotNode>& B)
			{
				return A->GetNodeName().ToString() < B->GetNodeName().ToString();
			});
		this->bHasInit = true;
	}
	return this->Children;
}

const TArray<TSharedPtr<FEventSlotNode>>& FEventSlotNode::GetFilteredChildNodes(const FString& FilterString)
{
	this->FilteredChildren = this->GetChildNodes().FilterByPredicate(
		[this, FilterString](const TSharedPtr<FEventSlotNode>& A) {
			return A->FilterByString(FilterString);
		});

	return this->FilteredChildren;
}

bool FEventSlotNode::FilterByString(const FString& FilterString) const
{
	bool bShow = this->GetNodeName().ToString().Contains(FilterString);

	if (!bShow)
	{
		for (const auto& Child : this->Children)
		{
			if (Child->FilterByString(FilterString))
			{
				bShow = true;
				break;
			}
		}
	}

	return bShow;
}

void FEventSlotNode::AddChild(TSharedPtr<FEventSlotNode> Data)
{
	check(!Data->Parent.IsValid())
	Data->Parent = this->AsWeak();
	this->Children.Add(Data);
}

void FEventSlotNode::SetSelected(const TSharedPtr<FEventSlotNode>& Selected)
{
	if (this->Children.Contains(Selected))
	{
		this->SelectedChild = Selected;
		if (this->Parent.IsValid())
		{
			this->Parent.Pin()->SetSelected(this->AsShared());
		}
	}
	else
	{
		this->SelectedChild.Reset();
		if (this->Parent.IsValid())
		{
			this->Parent.Pin()->SetSelected(nullptr);
		}
	}

	this->OnSelectionChanged();
}

TWeakPtr<FEventSlotNode> FEventSlotNode::GetRoot()
{
	if (this->Parent.IsValid())
	{
		return this->Parent.Pin()->GetRoot();
	}
	else
	{
		return this->AsWeak();
	}
}

FName FEventSlotNode::GetEventName() const
{
	if (this->SelectedChild.IsValid())
	{
		return this->SelectedChild->GetEventName();
	}
	else
	{
		return NAME_None;
	}
}

void FEventSlotNode::ClearSelection()
{
	TSharedPtr<FEventSlotNode> Current = this->GetRoot().Pin();

	while (Current.IsValid())
	{
		TSharedPtr<FEventSlotNode> Saved = Current->SelectedChild;

		Current->ClearNodeSelection();
		Current = Saved;
	}
}

void FEventSlotNode::ClearNodeSelection()
{
	this->SelectedChild = nullptr;
}

FName FEventSlotNode::GetNodeName() const
{
	return "DebugNodeName";
}

void FRootEventSlotNode::AddRoot(UObject* Obj)
{
	TSharedPtr<FEventSlotNode> Child = MakeShareable(new FAssetEventSlotNode(Obj));
	this->AddChild(Child);
}

FName FAssetEventSlotNode::GetNodeName() const
{
	if (this->Object.IsValid())
	{
		return this->DisplayText;
	}
	else
	{
		return "?Invalid Object?";
	}
}

FAssetEventSlotNode::FAssetEventSlotNode(UObject* Obj): Object(Obj)
{
	FString Short = Obj->GetName();

	if (Obj->IsA<UStateMachineInterface>())
	{
		if (Short.StartsWith("SMI_"))
		{
			this->DisplayText = FName(Short.RightChop(4));
		}
		else if (Short.EndsWith("_SMI"))
		{
			this->DisplayText = FName(Short.LeftChop(4));
		}
	}
	else if (Obj->IsA<UBlueprintGeneratedClass>())
	{
		if (Short.StartsWith("SM_"))
		{
			this->DisplayText = FName(Short.RightChop(3).LeftChop(2));
		}
		else if (Short.EndsWith("_SM_C"))
		{
			this->DisplayText = FName(Short.LeftChop(5));
		}
	}
}

void FRootEventSlotNode::Init()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassPaths.Add(UStateMachineInterface::StaticClass()->GetClassPathName());
	Filter.ClassPaths.Add(UStateMachine::StaticClass()->GetClassPathName());

	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssets(Filter, AssetDataList);

	for (const FAssetData& AssetData : AssetDataList)
	{
		this->AddRoot(AssetData.GetAsset());
	}

	for (TObjectIterator<UClass> It; It; ++It)
	{
		auto Class = *It;

		if (Class && Class->IsChildOf<UStateMachine>() && Cast<UStateMachineBlueprintGeneratedClass>(Class))
		{
			if (!Class->GetName().StartsWith("SKEL_"))
			{
				this->AddRoot(Class);
			}
		}
	}
}

void FAssetEventSlotNode::Init()
{
	if (auto SMI = Cast<UStateMachineInterface>(this->Object))
	{
		for (const auto& Data : SMI->GetEventData())
		{
			auto Child = MakeShareable(new FLeafEventSlotNode(Data.Key, Data.Value));
			this->AddChild(Child);
		}
	}
}

FLeafEventSlotNode::FLeafEventSlotNode(FName InName, const FSMIData& InData)
: EventName(InName),
	Data(InData)
{

}

void FLeafEventSlotNode::SelectEvent(bool bNewSelected)
{
	this->bSelected = bNewSelected;

	this->ClearSelection();
	this->GetParent()->SetSelected(this->AsShared());
}

void FLeafEventSlotNode::ClearNodeSelection()
{
	FEventSlotNode::ClearNodeSelection();
	this->bSelected = false;
}

FName FLeafEventSlotNode::GetEventName() const
{
	return this->EventName;
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
