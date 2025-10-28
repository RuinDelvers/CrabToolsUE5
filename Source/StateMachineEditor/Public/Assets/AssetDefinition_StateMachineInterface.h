#pragma once

#include "StateMachine/StateMachineInterface.h"
#include "AssetDefinitionDefault.h"

#include "AssetDefinition_StateMachineInterface.generated.h"

UCLASS()
class UAssetDefinition_StateMachineInterface : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:
	UAssetDefinition_StateMachineInterface();
	virtual ~UAssetDefinition_StateMachineInterface() override;
	
	// UAssetDefinition Begin
	virtual FText GetAssetDisplayName() const override;
	FText GetAssetDescription(const FAssetData& AssetData) const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	// UAssetDefinition End
};