#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/Nodes/AnimNode_CustomizableBoneTransform.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimGraphNode_CustomizableBoneTransform.generated.h"

class FCompilerResultsLog;

UCLASS(meta = (Keywords = "Modify Transform Customization"))
class UAnimGraphNode_CustomizableBoneTransform : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

	FNodeTitleTextTable CachedNodeTitles;

	UPROPERTY(EditAnywhere, Category="Settings")
	FAnimNode_CustomizableBoneTransform Node;

public:

	//UAnimGraphNode_CustomizableBoneTransform();
	UAnimGraphNode_CustomizableBoneTransform(const FObjectInitializer& Initializer);

	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface

protected:
	// UAnimGraphNode_Base interface
	virtual void ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog) override;
	virtual FEditorModeID GetEditorMode() const override;
	virtual void CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode) override;
	//virtual void CopyPinDefaultsToNodeData(UEdGraphPin* InPin) override;
	// End of UAnimGraphNode_Base interface

	// UAnimGraphNode_SkeletalControlBase interface
	virtual FText GetControllerDescription() const override;
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	// End of UAnimGraphNode_SkeletalControlBase interface

};