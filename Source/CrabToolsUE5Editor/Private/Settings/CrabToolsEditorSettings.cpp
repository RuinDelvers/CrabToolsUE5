#include "Settings/CrabToolsEditorSettings.h"
#include "Engine/DataTable.h"
#include "GameplayTagsManager.h"
#include "AssetRegistry/AssetRegistryModule.h"

UCrabToolsEditorSettings::UCrabToolsEditorSettings()
{
	FAssetRegistryModule::GetRegistry().OnAssetUpdated().AddUFunction(
		this,
		GET_FUNCTION_NAME_CHECKED(UCrabToolsEditorSettings, OnAssetsUpdated));

	FAssetRegistryModule::GetRegistry().OnAssetUpdatedOnDisk().AddUFunction(
		this,
		GET_FUNCTION_NAME_CHECKED(UCrabToolsEditorSettings, OnAssetsUpdated));
}

#if WITH_EDITOR

void UCrabToolsEditorSettings::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	if (Event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UCrabToolsEditorSettings, TagKeyTables))
	{
		for (const auto& Pair : this->TagKeyTables)
		{
			this->SyncTables(Pair.Key.LoadSynchronous(), Pair.Value.TagTable.LoadSynchronous());
		}
	}

	this->TryUpdateDefaultConfigFile();
}
#endif

void UCrabToolsEditorSettings::SyncTables(UDataTable* SourceTable, UDataTable* TagTable)
{
	if (!(SourceTable && TagTable)) { return; }

	FGameplayTag RootTag = this->TagKeyTables[SourceTable].RootTag;

	for (const auto& RowKey : SourceTable->GetRowNames())
	{
		

		if (TagTable->FindRowUnchecked(RowKey))
		{
			
		}
		else
		{
			FGameplayTagTableRow Row;
			Row.Tag = RowKey;
			TagTable->AddRow(RowKey, Row);
			TagTable->Modify();
		}

		if (RootTag.IsValid())
		{
			if (!FGameplayTag::RequestGameplayTag(RowKey).MatchesTag(RootTag))
			{
				UE_LOG(LogTemp, Error, TEXT("Found invalid row key %s in table %s"),
					*RowKey.ToString(),
					*SourceTable->GetName());
			}
		}
	}
}

void UCrabToolsEditorSettings::OnAssetsUpdated(const FAssetData& Data)
{
	if (auto Table = Cast<UDataTable>(Data.GetAsset()))
	{
		if (this->TagKeyTables.Contains(Data.GetAsset()))
		{
			this->SyncTables(Table, this->TagKeyTables[Table].TagTable.LoadSynchronous());
		}
	}	
}
