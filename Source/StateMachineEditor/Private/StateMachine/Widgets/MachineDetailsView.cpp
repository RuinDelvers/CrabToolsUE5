#include "StateMachine/Widgets/MachineDetailsView.h"
#include "StateMachine/StateMachineBlueprint.h"

#define LOCTEXT_NAMESPACE "PSM"

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
		//DetailsViewWidget->SetCustomFilterDelegate(FSimpleDelegate::CreateUObject(this, &UDetailsView::ToggleShowingOnlyAllowedProperties));
		Inspector->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateLambda([](const FPropertyAndParent& PropertyAndParent)
			{
				if (PropertyAndParent.Property.GetBoolMetaData("BlueprintPrivate"))
				{
					return false;
				}

				return true;
			}));
		//DetailsViewWidget->SetIsCustomRowVisibleDelegate(FIsCustomRowVisible::CreateUObject(this, &UDetailsView::GetIsRowVisible));
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