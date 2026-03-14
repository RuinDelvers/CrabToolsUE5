#include "Animation/Nodes/AnimGraphNode_CustomizableBoneTransform.h"
#include "AnimNodeEditModes.h"

#define LOCTEXT_NAMESPACE "CustomizableAnimNodes"


UAnimGraphNode_CustomizableBoneTransform::UAnimGraphNode_CustomizableBoneTransform(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


FText UAnimGraphNode_CustomizableBoneTransform::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
	{
		return GetControllerDescription();
	}
	else
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ControllerDescription"), GetControllerDescription());
		//Args.Add(TEXT("BoneName"), FText::FromName(Node.BoneToModify.BoneName));

		// FText::Format() is slow, so we cache this to save on performance
		if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
		{
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_CustomizableBoneTransform_ListTitle", "{ControllerDescription}"), Args), this);
		}
		else
		{
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_CustomizableBoneTransform_Title", "{ControllerDescription}"), Args), this);
		}
	}
	return CachedNodeTitles[TitleType];
}

FText UAnimGraphNode_CustomizableBoneTransform::GetControllerDescription() const
{
	return LOCTEXT("TransformBoneCustomizer", "Customizable Bone Transform");
}

FEditorModeID UAnimGraphNode_CustomizableBoneTransform::GetEditorMode() const
{
	return AnimNodeEditModes::AnimNode;
}

FText UAnimGraphNode_CustomizableBoneTransform::GetTooltipText() const
{
	return LOCTEXT(
		"AnimGraphNode_CustomizableBoneTransform_Tooltip",
		"Transform a Bone. This can be customized.");
}

void UAnimGraphNode_CustomizableBoneTransform::ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog)
{
	if (ForSkeleton && !ForSkeleton->HasAnyFlags(RF_NeedPostLoad))
	{
		this->Node.ForEachBone(FAnimNode_CustomizableBoneTransform::FPerDataPointDelegate::CreateLambda(
			[this, ForSkeleton, &MessageLog](const FAnimNode_CustomizableBoneTransformDataPoint& Point)
			{
				if (ForSkeleton->GetReferenceSkeleton().FindBoneIndex(Point.BoneToModify.BoneName) == INDEX_NONE)
				{
					if (Point.BoneToModify.BoneName == NAME_None)
					{
						MessageLog.Warning(*LOCTEXT("NoBoneSelectedToModify", "@@ - You must pick a bone to modify").ToString(), this);
					}
					else
					{
						FFormatNamedArguments Args;
						Args.Add(TEXT("BoneName"), FText::FromName(Point.BoneToModify.BoneName));

						FText Msg = FText::Format(LOCTEXT("NoBoneFoundToModify", "@@ - Bone {BoneName} not found in Skeleton"), Args);

						MessageLog.Warning(*Msg.ToString(), this);
					}
				}

				if ((Point.TranslationMode == BMM_Ignore) && (Point.RotationMode == BMM_Ignore) && (Point.ScaleMode == BMM_Ignore))
				{
					MessageLog.Warning(*LOCTEXT("NothingToModify", "@@ - No components to modify selected.  Either Rotation, Translation, or Scale should be set to something other than Ignore").ToString(), this);
				}
			}));
	}

	Super::ValidateAnimNodeDuringCompilation(ForSkeleton, MessageLog);
}

void UAnimGraphNode_CustomizableBoneTransform::CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode)
{	FAnimNode_CustomizableBoneTransform* CustomizableBoneTransform = static_cast<FAnimNode_CustomizableBoneTransform*>(InPreviewNode);

	CustomizableBoneTransform->SetData(this->Node);
}


#undef LOCTEXT_NAMESPACE