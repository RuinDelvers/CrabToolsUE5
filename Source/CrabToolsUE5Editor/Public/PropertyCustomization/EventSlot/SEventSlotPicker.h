#pragma once

#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "StateMachine/StateMachineInterface.h"

class IPropertyHandle;
class SMenuAnchor;
class SButton;
class SComboButton;
struct FSMIData;

/* Node for keeping track of the event slot display tree. */
struct FEventSlotNode : public TSharedFromThis<FEventSlotNode>
{

private:

	TWeakPtr<FEventSlotNode> Parent;
	TArray<TSharedPtr<FEventSlotNode>> Children;
	TArray<TSharedPtr<FEventSlotNode>> FilteredChildren;
	bool bHasInit = false;
	TSharedPtr<FEventSlotNode> SelectedChild;

public:

	virtual ~FEventSlotNode() {}

	const TArray<TSharedPtr<FEventSlotNode>>& GetChildNodes();
	const TArray<TSharedPtr<FEventSlotNode>>& GetFilteredChildNodes(const FString& FilterString);
	int ChildCount() const { return Children.Num(); }
	void ClearSelection();
	TWeakPtr<FEventSlotNode> GetRoot();
	TSharedPtr<FEventSlotNode> GetParent() { return this->Parent.Pin(); }
	const TSharedPtr<FEventSlotNode>& GetSelectedChild() const { return this->SelectedChild; }

	bool FilterByString(const FString& FilterString) const;

	virtual FName GetEventName() const;
	virtual void SelectEvent(bool bNewSelected) {}
	virtual bool IsSelected() const { return SelectedChild.IsValid(); }
	virtual FName GetNodeName() const;	
	virtual bool CanBeSelected() const { return false; }

	void SetSelected(const TSharedPtr<FEventSlotNode>& Selected);
	virtual void ClearNodeSelection();
	const TArray<TSharedPtr<FEventSlotNode>> GetFilteredChildren() const { return this->FilteredChildren; }

protected:
	
	virtual void OnSelectionChanged() {}
	virtual void Init() {};
	void AddChild(TSharedPtr<FEventSlotNode> Data);
};

class FAssetEventSlotNode : public FEventSlotNode
{
	TWeakObjectPtr<UObject> Object;
	FName DisplayText;

public:

	virtual ~FAssetEventSlotNode() {}

	virtual FName GetEventName() const override;
	FAssetEventSlotNode(UObject* Obj);
	virtual FName GetNodeName() const override;

protected:

	virtual void Init() override;
};

class FRootEventSlotNode : public FEventSlotNode
{
public:

	DECLARE_MULTICAST_DELEGATE(FSelectionUpdated)
	FSelectionUpdated OnSelectionUpdated;

public:

	virtual ~FRootEventSlotNode() {}
	void AddRoot(UObject* Obj);	
	

protected:

	virtual void OnSelectionChanged() override { this->OnSelectionUpdated.Broadcast(); }
	virtual void Init() override;

private:
	
};

class FLeafEventSlotNode : public FEventSlotNode
{

private:

	FName EventName;
	FSMIData Data;
	bool bSelected = false;

public:

	FLeafEventSlotNode(FName InName, const FSMIData& InData);
	virtual ~FLeafEventSlotNode() {}

	virtual FName GetEventName() const override;
	virtual void SelectEvent(bool bNewSelected) override;
	virtual bool IsSelected() const { return this->bSelected; }
	virtual bool CanBeSelected() const override { return true; }
	virtual FName GetNodeName() const override { return this->EventName; }
	virtual void ClearNodeSelection() override;

protected:

	
	
private:


};

/**
 * Widget for selecting events from various assets.
 */
class SEventSlotPicker : public SCompoundWidget
{

	/** Called when a tag status is changed */
	DECLARE_DELEGATE_OneParam(FOnSlotChanged, FEventSlot)

	/** Called on when tags might need refreshing (e.g. after undo/redo or when tags change). Use SetTagContainers() to set the new tags. */
	DECLARE_DELEGATE_OneParam(FOnRefreshSlot, SEventSlotPicker&)

	enum class ESlotFilterResult
	{
		IncludeTag,
		ExcludeTag
	};

	DECLARE_DELEGATE_RetVal_OneParam(ESlotFilterResult, FOnFilterSlot, const TSharedPtr<FEventSlotNode>&)

	/* Holds the Name of this TagContainer used for saving out expansion settings */
	FString SettingsName;

	/* Filter string used during search box */
	FString FilterString;

	/** User specified filter function. */
	FOnFilterSlot SlotFilter;

	/* Flag to set if the list is read only*/
	bool bReadOnly = false;

	/** The maximum height of the gameplay tag tree. If 0, the height is unbound. */
	float MaxHeight = 260.0f;

	/* Array of all tags */
	//TArray<TSharedPtr<FEventSlotNode>> TagItems;
	TSharedPtr<FRootEventSlotNode> Root;

	/* Array of tags filtered to be displayed in the TreeView */
	TArray<TSharedPtr<FEventSlotNode>> FilteredTagItems;

	/** Container widget holding the tag tree */
	TSharedPtr<SBorder> TagTreeContainerWidget;

	/** Tree widget showing the gameplay tag library */
	TSharedPtr<STreeView<TSharedPtr<FEventSlotNode>>> TagTreeWidget;

	/** Allows for the user to find a specific gameplay tag in the tree */
	TSharedPtr<SSearchBox> SearchTagBox;

	/** Property to edit */
	TSharedPtr<IPropertyHandle> PropertyHandle;

	/** Containers to modify, ignored if PropertyHandle is set. */
	//TArray<FGameplayTagContainer> TagContainers;

	/** Called when the Tag list changes*/
	FOnSlotChanged OnSlotChanged;

	/** Called on when tags might need refreshing (e.g. after undo/redo or when tags change). */
	FOnRefreshSlot OnRefreshTagContainers;

	/** Tag to scroll to, cleared after scroll is requested. */
	FEventSlot RequestedScrollToTag;

	/** Determines behavior of the widget */
	//EGameplayTagPickerMode GameplayTagPickerMode = EGameplayTagPickerMode::SelectionMode;

	/** Guard value used to prevent feedback loops in selection handling. */
	bool bInSelectionChanged = false;

	/** Guard value used define if expansion code should persist the expansion operations. */
	bool bPersistExpansionChange = true;


	//TSharedPtr<SAddNewGameplayTagWidget> AddNewTagWidget;
	bool bNewTagWidgetVisible = false;

	FDelegateHandle PostUndoRedoDelegateHandle;

	SLATE_BEGIN_ARGS(SEventSlotPicker)
		: _Filter()
		, _ReadOnly(false)
		, _ShowMenuItems(false)
		, _PropertyHandle(nullptr)
		//, _GameplayTagPickerMode(EGameplayTagPickerMode::SelectionMode)
		, _MaxHeight(260.0f)
		, _Padding(FMargin(2.0f))
		{
		}
		// Comma delimited string of tag root names to filter by
		SLATE_ARGUMENT(FString, Filter)

		// Optional filter function called when generating the tag list
		SLATE_EVENT(FOnFilterSlot, OnFilterSlot)

		SLATE_EVENT(FOnSlotChanged, OnSlotChanged)

		// The name that will be used for the settings file
		SLATE_ARGUMENT(FString, SettingsName)

		// Flag to set if the list is read only
		SLATE_ARGUMENT(bool, ReadOnly)

		// If set, wraps the picker in a menu builder and adds common menu commands.
		SLATE_ARGUMENT(bool, ShowMenuItems)

		// Property handle to FGameplayTag or FGameplayTagContainer, used to get and modify the edited tags.
		SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, PropertyHandle)

		// Tags or tag containers to modify. If MultiSelect is false, the container will container single tags.
		// If PropertyHandle is set, the tag containers will be ignored.
		//SLATE_ARGUMENT(TArray<FGameplayTagContainer>, TagContainers)

		// Determines behavior of the menu based on where it's used
		//SLATE_ARGUMENT(EGameplayTagPickerMode, GameplayTagPickerMode)

		// Caps the height of the gameplay tag tree, 0.0 means uncapped.
		SLATE_ARGUMENT(float, MaxHeight)

		// Padding inside the picker widget
		SLATE_ARGUMENT(FMargin, Padding)

		// Called on when tags might need refreshing (e.g. after undo/redo or when tags change).
		SLATE_EVENT(FOnRefreshSlot, OnRefreshTagContainers)
	SLATE_END_ARGS()

	/**
	 * Given a property handle, try and enumerate the tag containers from within it (when dealing with a struct property of type FGameplayTag or FGameplayTagContainer).
	 * @return True if it was possible to enumerate containers (even if no containers were enumerated), or false otherwise.
	 */
	//static bool EnumerateEditableTagContainersFromPropertyHandle(const TSharedRef<IPropertyHandle>& PropHandle, TFunctionRef<bool(const FGameplayTagContainer&)> Callback);

	/**
	 * Given a property handle, try and extract the editable tag containers from within it (when dealing with a struct property of type FGameplayTagContainer).
	 * @return True if it was possible to extract containers (even if no containers were extracted), or false otherwise.
	 */
	//static bool GetEditableTagContainersFromPropertyHandle(const TSharedRef<IPropertyHandle>& PropHandle, TArray<FGameplayTagContainer>& OutEditableContainers);

	virtual ~SEventSlotPicker() override;

	/** Construct the actual widget */
	void Construct(const FArguments& InArgs);

	/** Updates the tag list when the filter text changes */
	void OnFilterTextChanged(const FText& InFilterText);

	/** Forces the widget to refresh its tags on the next tick */
	void RefreshOnNextTick();

	/** Scrolls the view to specified tag. */
	void RequestScrollToView(const FEventSlot Tag);

	/** Gets the widget to focus once the menu opens. */
	TSharedPtr<SWidget> GetWidgetToFocusOnOpen();

private:

	enum class EGameplayTagAdd : uint8
	{
		Root,
		Child,
		Duplicate,
	};

	void OnPostUndoRedo();


	/* Filters the tree view based on the current filter text. */
	void FilterTagTree();

	/**
	 * Generate a row widget for the specified item node and table
	 *
	 * @param InItem		Tag node to generate a row widget for
	 * @param OwnerTable	Table that owns the row
	 *
	 * @return Generated row widget for the item node
	 */
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FEventSlotNode> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	/**
	 * Get children nodes of the specified node
	 *
	 * @param InItem		Node to get children of
	 * @param OutChildren	[OUT] Array of children nodes, if any
	 */
	void OnGetChildren(TSharedPtr<FEventSlotNode> InItem, TArray< TSharedPtr<FEventSlotNode> >& OutChildren);

	/**
	 * Called via delegate when the status of a check box in a row changes
	 *
	 * @param NewCheckState	New check box state
	 * @param NodeChanged	Node that was checked/unchecked
	 */
	void OnNodeCheckStatusChanged(ECheckBoxState NewCheckState, TSharedPtr<FEventSlotNode> NodeChanged);

	/**
	 * Called via delegate to determine the checkbox state of the specified node
	 *
	 * @param Node	Node to find the checkbox state of
	 *
	 * @return Checkbox state of the specified node
	 */
	ECheckBoxState IsTagChecked(TSharedPtr<FEventSlotNode> Node) const;

	/** Helper function to determine the visibility of the checkbox for allowing non-restricted children of restricted gameplay tags */
	EVisibility DetermineAllowChildrenVisible(TSharedPtr<FEventSlotNode> Node) const;

	/**
	 * Recursive function to uncheck all child tags
	 *
	 * @param NodeUnchecked	Node that was unchecked by the user
	 * @param EditableContainer The container we are removing the tags from
	 */
	//void UncheckChildren(TSharedPtr<FEventSlotNode> NodeUnchecked, FGameplayTagContainer& EditableContainer);

	/**
	 * Called via delegate to determine the text colour of the specified node
	 *
	 * @param Node	Node to find the colour of
	 *
	 * @return Text colour of the specified node
	 */
	FSlateColor GetTagTextColour(TSharedPtr<FEventSlotNode> Node) const;

	/** Called when the user clicks the "Clear All" button; Clears all tags */
	void OnMenuClearClicked(TSharedPtr<SComboButton> OwnerCombo);

	/** Called when the user clicks the "Expand All" button; Expands the entire tag tree */
	void OnExpandAllClicked(TSharedPtr<SComboButton> OwnerCombo);

	/** Called when the user clicks the "Collapse All" button; Collapses the entire tag tree */
	void OnCollapseAllClicked(TSharedPtr<SComboButton> OwnerCombo);

	/** Recursive function to go through all tags in the tree and set the expansion to default*/
	void SetDefaultTagNodeItemExpansion(TSharedPtr<FEventSlotNode> Node);

	/** Expansion changed callback */
	void OnExpansionChanged(TSharedPtr<FEventSlotNode> InItem, bool bIsExpanded);

	FText GetHighlightText() const;

	/** Creates a dropdown menu to provide additional functionality for tags (renaming, deletion, search for references, etc.) */
	TSharedRef<SWidget> MakeEventActionsMenu(TSharedPtr<FEventSlotNode> InTagNode, TSharedPtr<SComboButton> OwnerCombo, const bool bInShouldCloseWindowAfterMenuSelection);

	/** Creates settings menu content. */
	TSharedRef<SWidget> MakeSettingsMenu(TSharedPtr<SComboButton> OwnerCombo);

	/** Searches for all references for the selected tag */
	void OnSearchForReferences(TSharedPtr<FEventSlotNode> InTagNode, TSharedPtr<SComboButton> OwnerCombo);

	/** Searches for all references for the selected tag and its children */
	void OnSearchForReferencesAsChildren(TSharedPtr<FEventSlotNode> InTagNode, TSharedPtr<SComboButton> OwnerCombo);

	/** Copies individual tag's name to clipboard. */
	void OnCopyTagNameToClipboard(TSharedPtr<FEventSlotNode> InTagNode, TSharedPtr<SComboButton> OwnerCombo);

	/** Called to create context menu for the tag tree items. */
	TSharedPtr<SWidget> OnTreeContextMenuOpening();

	/** Called when the tag tree selection changes. */
	void OnTreeSelectionChanged(TSharedPtr<FEventSlotNode> SelectedItem, ESelectInfo::Type SelectInfo);

	/** Called to handle key presses in the tag tree. */
	FReply OnTreeKeyDown(const FGeometry& Geometry, const FKeyEvent& Key);

private:

	void OnEventSelectionChanged();
	void InitRoot();

	void SetEventName(FName InEvent);

	void ExpandFullTree(const TSharedPtr<FEventSlotNode>& Item, bool bExpansion);
};