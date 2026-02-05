#include "StateMachine/Widgets/MachineDetailsView.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "EditorCategoryUtils.h"

#define LOCTEXT_NAMESPACE "PSM"

bool SMachineDetailsView::PropertyFilterRule(const FPropertyAndParent& PropertyAndParent)
{
	auto Properties = PropertyAndParent.Property.GetMetaDataMap();
	
	// If private, do not show at all.
	if (PropertyAndParent.Property.GetBoolMetaData("BlueprintPrivate"))
	{
		return false;
	}

	if (auto Class = PropertyAndParent.Property.GetOwnerClass())
	{
		if (PropertyAndParent.Property.GetOwnerClass()->IsChildOf<UStateNode>())
		{
			// Hide emitted events for this viewer, as editing it would be incorrect.
			if (PropertyAndParent.Property.GetFName() == "EmittedEvents")
			{
				return false;
			}

			// State nodes will use hide categories to control view of properties they use, but don't want editable.
			if (Properties)
			{
				if (auto Category = Properties->Find("Category"))
				{
					TArray<FString> ClassHideCategories;
					FEditorCategoryUtils::GetClassHideCategories(Class, ClassHideCategories);

					if (ClassHideCategories.Contains(*Category))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

void SMachineDetailsView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InEditor) 
{
	auto WidgetName = FName("SMachineDetailsView");

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
		Inspector->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateStatic(&SMachineDetailsView::PropertyFilterRule));
	}

	ChildSlot
	[
		this->Inspector.ToSharedRef()
	];
	this->BindEvents(InEditor);
}

void SMachineDetailsView::BindEvents(TSharedPtr<class FEditor> InEditor)
{
	if (auto BPObj = InEditor->GetStateMachineBlueprintObj())
	{
		auto Graph = BPObj->GetMainGraph();

		BPObj->Events.OnObjectInspected.AddSP(this, &SMachineDetailsView::InspectObject);
	}
}

void SMachineDetailsView::InspectObject(UObject* Obj)
{
	Inspector->SetObject(Obj);
	
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