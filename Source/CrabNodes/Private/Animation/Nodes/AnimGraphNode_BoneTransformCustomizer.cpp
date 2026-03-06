#include "Animation/Nodes/AnimGraphNode_BoneTransformCustomizer.h"
#include "AnimNodeEditModes.h"

#define LOCTEXT_NAMESPACE "CustomizableAnimNodes"

UAnimGraphNode_BoneTransformCustomizer::UAnimGraphNode_BoneTransformCustomizer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UAnimGraphNode_BoneTransformCustomizer::GetNodeTitle(ENodeTitleType::Type TitleType) const
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
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_BoneTransformCustomizer_ListTitle", "{ControllerDescription}"), Args), this);
		}
		else
		{
			CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_BoneTransformCustomizer_Title", "{ControllerDescription}"), Args), this);
		}
	}
	return CachedNodeTitles[TitleType];
}

FText UAnimGraphNode_BoneTransformCustomizer::GetTooltipText() const
{
	return LOCTEXT(
		"AnimGraphNode_BoneTransformCustomizer_Tooltip",
		"Transform a Bone. This can be customized.");
}

void UAnimGraphNode_BoneTransformCustomizer::ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog)
{
	if (this->Node.IsEmpty())
	{
		MessageLog.Warning(*LOCTEXT("NoBoneSelectedToModify", "@@ - There are no transforms specified.").ToString(), this);
	}
	else
	{
		if (ForSkeleton && !ForSkeleton->HasAnyFlags(RF_NeedPostLoad))
		{
			this->Node.ForEachBone(FAnimNode_BoneTransformCustomizer::FPerDataPointDelegate::CreateLambda(
				[this, ForSkeleton, &MessageLog](const FAnimNode_BoneTransformCustomizerDataPoint& Point)
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
	}

	Super::ValidateAnimNodeDuringCompilation(ForSkeleton, MessageLog);
}


FEditorModeID UAnimGraphNode_BoneTransformCustomizer::GetEditorMode() const
{
	return AnimNodeEditModes::AnimNode;
}


void UAnimGraphNode_BoneTransformCustomizer::CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode)
{
	UE_LOG(LogTemp, Warning, TEXT("Copying node data to preview"));
	FAnimNode_BoneTransformCustomizer* BoneTransformCustomizer = static_cast<FAnimNode_BoneTransformCustomizer*>(InPreviewNode);

	BoneTransformCustomizer->SetData(this->Node);
}

FText UAnimGraphNode_BoneTransformCustomizer::GetControllerDescription() const
{
	return LOCTEXT("TransformBoneCustomizer", "Bone Transform Customizer");
}

#undef LOCTEXT_NAMESPACE