#include "StateMachine/Widgets/MachineDetailsView.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "EditorCategoryUtils.h"
#include "Kismet2/SClassPickerDialog.h"
#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"
#include "Widgets/Layout/SScrollBox.h"
#include "StateMachine/Widgets/StateNodes/SBaseStateNode.h"
#include "StateMachine/Widgets/Utils.h"

#define LOCTEXT_NAMESPACE "PSM"

class FStateNodeClassFilter : public IClassViewerFilter
{
public:
	/** All children of these classes will be included unless filtered out by another setting. */
	TSet <const UClass*> AllowedChildrenOfClasses;

	/** Disallowed class flags. */
	EClassFlags DisallowedClassFlags;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return !InClass->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed
			&& !InClass->IsChildOf<UAbstractCondition>();
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}

};

void SMachineDetailsView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InEditor) 
{
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

		FDetailsViewArgs DetailsViewArgs;
		DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
		DetailsViewArgs.bUpdatesFromSelection = false;
		DetailsViewArgs.bLockable = false;
		DetailsViewArgs.bShowPropertyMatrixButton = false;
		DetailsViewArgs.NotifyHook = this;

		DetailsViewArgs.ViewIdentifier = "Machine Details";
		DetailsViewArgs.bAllowSearch = true;
		DetailsViewArgs.bAllowFavoriteSystem = true;
		DetailsViewArgs.bShowOptions = true;
		DetailsViewArgs.bShowModifiedPropertiesOption = true;
		DetailsViewArgs.bShowKeyablePropertiesOption = true;
		DetailsViewArgs.bShowAnimatedPropertiesOption = false;
		DetailsViewArgs.bShowScrollBar = true;
		DetailsViewArgs.bForceHiddenPropertyVisibility = false;
		DetailsViewArgs.ColumnWidth = 0.3f;
		DetailsViewArgs.bShowCustomFilterOption = true;
		
		Inspector = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

		Inspector->SetCustomFilterLabel(LOCTEXT("ShowAllParameters", "Show All Parameters"));
		Inspector->SetIsPropertyVisibleDelegate(
			FIsPropertyVisible::CreateStatic(
				&StateMachineEditor::Widgets::DetailWidgetPropertyFilter));
	}

	FSlateFontInfo Font = FAppStyle::GetFontStyle("DetailsView.CategoryTextStyle");
	Font.Size = 28;

	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		.AutoSize()
		[
			SNew(SVerticalBox)
				// Inspector for the EdState node and other selectable objects.
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					this->Inspector.ToSharedRef()
				]
				// Section header for the nodes.
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				.Padding(0, 1, 0, 1)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryTop"))
					.Visibility(this, &SMachineDetailsView::StateNodesVisibility)
					[
						SNew(SOverlay)
							+ SOverlay::Slot()
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
									.Text(FText::FromString("State Nodes"))
									.Font(Font)
							]
							+ SOverlay::Slot()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Center)
							[
								SNew(SButton)
									.OnClicked(this, &SMachineDetailsView::OnAddStateNode)
									.ButtonStyle(FAppStyle::Get(), "SimpleButton")
									.Text(FText::FromString("Delete"))
									.ContentPadding(FMargin(1, 0))
									[
										SNew(SImage)
											.Image(FAppStyle::Get().GetBrush("Icons.PlusCircle"))
											.DesiredSizeOverride(FVector2d(48, 48))
											.ColorAndOpacity(FSlateColor::UseForeground())
									]
							]
					]
				]
				// Section for state node view.
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(StateNodeContainer, SVerticalBox)
				]
		]
	];
	this->BindEvents(InEditor);
}

void SMachineDetailsView::BindEvents(TSharedPtr<class FEditor> InEditor)
{
	if (auto BPObj = InEditor->GetStateMachineBlueprintObj())
	{
		BPObj->Events.OnObjectInspected.AddSP(this, &SMachineDetailsView::InspectObject);
	}
}

void SMachineDetailsView::InspectObject(UObject* Obj)
{
	this->StateNodeContainer->ClearChildren();
	Inspector->SetObject(Obj);
	
	if (auto EdStateNode = Cast<UEdStateNode>(Obj))
	{
		if (this->InspectedStateNode.IsValid())
		{
			this->InspectedStateNode->Events.OnStateNodesUpdated.RemoveAll(this);
		}

		this->InspectedStateNode = EdStateNode;

		for (const auto& Node : EdStateNode->GetNodes())
		{
			auto NodeInspector = SNew(SBaseStateNodeDetails, EdStateNode, Node);
			this->StateNodeContainer->AddSlot()
				.AutoHeight()
				.Padding(0, 1, 0, 1)
				.AttachWidget(NodeInspector);
		}

		EdStateNode->Events.OnStateNodesUpdated.AddSP(this, &SMachineDetailsView::OnStateNodesChanged);
	}
	else
	{
		this->InspectedStateNode = nullptr;
	}
}

FReply SMachineDetailsView::OnAddStateNode()
{
	FClassViewerModule& ClassViewerModule =
		FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	FClassViewerInitializationOptions Options;
	Options.DisplayMode = EClassViewerDisplayMode::Type::TreeView;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.bShowNoneOption = false;
	Options.bExpandAllNodes = true;

	TSharedPtr<FStateNodeClassFilter> Filter = MakeShareable(new FStateNodeClassFilter);
	Options.ClassFilters.Add(Filter.ToSharedRef());

	/*
	if (Options.ExtraPickerCommonClasses.Num() == 0)
	{
		Options.ExtraPickerCommonClasses.Add(UStateNode::StaticClass());
	}
	*/

	Filter->DisallowedClassFlags = CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_Hidden | CLASS_HideDropDown | CLASS_Abstract;
	Filter->AllowedChildrenOfClasses.Add(UStateNode::StaticClass());

	const FText TitleText = LOCTEXT("CreateStateNode", "Pick a Node Class");

	UClass* ChosenParentClass = nullptr;
	bool isSuccessful = SClassPickerDialog::PickClass(TitleText, Options, ChosenParentClass, UStateNode::StaticClass());
	
	if (isSuccessful && ChosenParentClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding node of class %s"), *ChosenParentClass->GetName());
		if (auto Node = Cast<UEdStateNode>(this->InspectedStateNode))
		{
			Node->AddStateNode(ChosenParentClass);
		}
	}

	return FReply::Handled();
}

void SMachineDetailsView::OnStateNodesChanged(UEdBaseNode* NodeUpdated)
{
	UE_LOG(LogTemp, Warning, TEXT("state nodes changed."));
	this->InspectObject(NodeUpdated);
}

EVisibility SMachineDetailsView::StateNodesVisibility() const
{
	if (this->Inspector->GetSelectedObjects().Num() > 0)
	{
		return EVisibility::Visible;
	}
	else
	{
		return EVisibility::Collapsed;
	}
}

void SMachineDetailsView::AddReferencedObjects(FReferenceCollector& Collector)
{

}

void SMachineDetailsView::OnSelectionChanged(TArray<class UEdGraphNode*>& SelectedNodes)
{
	if (SelectedNodes.Num() == 1 && SelectedNodes[0])
	{
		Inspector->SetObject(SelectedNodes[0]);
	}
	else
	{
		Inspector->SetObject(nullptr);
	}
}


#undef LOCTEXT_NAMESPACE