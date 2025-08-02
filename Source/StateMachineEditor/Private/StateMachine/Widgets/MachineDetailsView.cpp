#include "StateMachine/Widgets/MachineDetailsView.h"
#include "StateMachine/StateMachineBlueprint.h"

#define LOCTEXT_NAMESPACE "PSM"

bool SMachineDetailsView::PropertyFilterRule(const FPropertyAndParent& PropertyAndParent)
{
	/*
	auto Properties = PropertyAndParent.Property.GetMetaDataMap();

	if (Properties)
	{
		UE_LOG(LogTemp, Warning, TEXT("Property Name: %s"), *PropertyAndParent.Property.GetName())
		for (const auto& Pair : *Properties)
		{
			UE_LOG(LogTemp, Warning, TEXT("Key = %s, Value = %s"), *Pair.Key.ToString(), *Pair.Value);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("------------------------------------"));
	*/

	if (PropertyAndParent.Property.GetBoolMetaData("BlueprintPrivate"))
	{
		return false;
	}
	if (PropertyAndParent.Property.GetOwnerClass() != nullptr && PropertyAndParent.Property.GetOwnerClass()->IsChildOf<UStateNode>())
	{
		if (PropertyAndParent.Property.GetFName() == "EmittedEvents")
		{
			return false;
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
		//DetailsViewWidget->SetCustomFilterDelegate(FSimpleDelegate::CreateUObject(this, &UDetailsView::ToggleShowingOnlyAllowedProperties));
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