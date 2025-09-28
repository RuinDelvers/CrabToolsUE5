#pragma once

#include "AssetDefinitionDefault.h"
#include "AssetDefinition_StateMachineBlueprint.generated.h"

UCLASS()
class UAssetDefinition_StateMachineBlueprint : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:
	UAssetDefinition_StateMachineBlueprint();
	virtual ~UAssetDefinition_StateMachineBlueprint() override;
	
	// UAssetDefinition Begin
	virtual FText GetAssetDisplayName() const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	virtual EAssetCommandResult OpenAssets(const FAssetOpenArgs& OpenArgs) const override;
	virtual EAssetCommandResult PerformAssetDiff(const FAssetDiffArgs& DiffArgs) const override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
	// UAssetDefinition End
};