#include "CrabToolsUE5EditorModule.h"
#include "Selection.h"
#include "Actors/Paths/PatrolPath.h"
#include "Actors/DynamicActorSpawner.h"
#include "PropertyCustomization/EventSlot/EventSlotCustomization.h"
#include "Styles/StyleRoot.h"

#define LOCTEXT_NAMESPACE "FCrabToolsUE5EditorModule"

const FName CrabToolsUE5EditorAppIdentifier = FName(TEXT("CrabToolsUE5EditorApp"));

FCrabToolsUE5EditorModule::FCrabToolsUE5EditorModule()
{

}

void FCrabToolsUE5EditorModule::StartupModule() 
{
	this->SelectionChangedHandle = USelection::SelectionChangedEvent.AddRaw(
		this,
		&FCrabToolsUE5EditorModule::OnSelectionChanged);

	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FCrabToolsUE5EditorModule::OnPostEngineInit);
	StyleRoot::InitializeStyles();
}

void FCrabToolsUE5EditorModule::ShutdownModule() 
{
	USelection::SelectionChangedEvent.Remove(SelectionChangedHandle);
}

FString FCrabToolsUE5EditorModule::GetReferencerName() const  {
	return "CrabToolsUE5EditorModule";
}

void FCrabToolsUE5EditorModule::OnPostEngineInit()
{
	this->InitializeCustomization();
}

void FCrabToolsUE5EditorModule::InitializeCustomization()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("EventSlot", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FEventSlotCustomization::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FCrabToolsUE5EditorModule::OnSelectionChanged(UObject* Obj)
{
	if (auto PatrolPath = Cast<APatrolPath>(Obj))
	{
		PatrolPath->ToggleDisplay();
	}
	else if (auto ActorSpawner = Cast<ADynamicActorSpawner>(Obj))
	{
		ActorSpawner->ToggleDisplay();
	}

	if (auto Selection = Cast<USelection>(Obj))
	{
		TArray<UObject*> Selected;
		Selection->GetSelectedObjects(Selected);

		TSet<APatrolPath*> SelectedPaths;
		TSet<ADynamicActorSpawner*> SelectedSpawners;

		for (auto& SelectedObj : Selected)
		{
			if (auto PatrolPath = Cast<APatrolPath>(SelectedObj))
			{
				SelectedPaths.Add(PatrolPath);
				PatrolPath->ToggleDisplay();
			}
			else if (auto ActorSpawner = Cast<ADynamicActorSpawner>(SelectedObj))
			{
				SelectedSpawners.Add(ActorSpawner);
				ActorSpawner->ToggleDisplay();
			}
		}	

		for (auto& OldPath : this->OldSelectedPatrolPaths)
		{
			if (IsValid(OldPath.Get()))
			{
				OldPath.Get()->ToggleDisplay();
			}
		}

		for (auto& OldSpawner : this->OldSelectedSpawners)
		{
			if (IsValid(OldSpawner))
			{
				OldSpawner->ToggleDisplay();
			}
		}

		this->OldSelectedPatrolPaths = SelectedPaths;
		this->OldSelectedSpawners = SelectedSpawners;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCrabToolsUE5EditorModule, CrabToolsUE5Editor)