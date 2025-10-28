#include "Assets/AssetDefinition_StateMachineInterface.h"

#include "StateMachine/Editor.h"
#include "StateMachine/StateMachineInterface.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UAssetDefinition_StateMachineInterface::UAssetDefinition_StateMachineInterface() = default;

UAssetDefinition_StateMachineInterface::~UAssetDefinition_StateMachineInterface() = default;

FText UAssetDefinition_StateMachineInterface::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_StateMachineInterface", "State Machine Interface");
}

FLinearColor UAssetDefinition_StateMachineInterface::GetAssetColor() const
{
	return FLinearColor(FColor(200, 116, 0));
}

TSoftClassPtr<> UAssetDefinition_StateMachineInterface::GetAssetClass() const
{
	return UStateMachineInterface::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_StateMachineInterface::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath, TFixedAllocator<1>> Categories = { EAssetCategoryPaths::AI };
	return Categories;
}

FText UAssetDefinition_StateMachineInterface::GetAssetDescription(const FAssetData& AssetData) const
{
	FString Description = AssetData.GetTagValueRef<FString>(GET_MEMBER_NAME_CHECKED(UStateMachineInterface, Description));

	if (!Description.IsEmpty())
	{
		Description.ReplaceInline(TEXT("\\n"), TEXT("\n"));
		return FText::FromString(MoveTemp(Description));
	}

	return FText::GetEmpty();
}


#undef LOCTEXT_NAMESPACE