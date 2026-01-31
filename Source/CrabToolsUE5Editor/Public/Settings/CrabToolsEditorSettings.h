#pragma once

#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "CrabToolsEditorSettings.generated.h"

class UDataTable;

USTRUCT(BlueprintType)
struct FGameplayTagValueStruct
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tags")
	TSoftObjectPtr<UDataTable> TagTable;

	/*
	 * If this is set to a valid tag, then every key in the key table will ensure that
	 * its keys are relative to this tag.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
	FGameplayTag RootTag;
};


/**
 *
 */
UCLASS(config = Editor, DisplayName = "CrabTools Editor Settings")
class CRABTOOLSUE5EDITOR_API UCrabToolsEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	/*
	 * This map of data tables will map a set of data tables to GameplayTagTableRows
	 * so that each key of the key DataTables exist as a GameplayTag in the value tables.
	 * 
	 * The map maps data tables to a table containing a gameplay tag data table.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "GameplayTags",
		meta=(ForceInlineRow))
	TMap<TSoftObjectPtr<UDataTable>, FGameplayTagValueStruct> TagKeyTables;

public:

	UCrabToolsEditorSettings();

protected:

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
	#endif

private:

	void SyncTables(UDataTable* SourceTable, UDataTable* TagTable);

	UFUNCTION()
	void OnAssetsUpdated(const FAssetData& Data);
};