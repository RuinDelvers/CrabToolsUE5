#include "Animation/Nodes/AnimNode_CustomizableBoneTransform.h"
#include "Animation/CustomizableAnimInstance.h"
#include "Animation/AnimInstanceProxy.h"

FAnimNode_CustomizableBoneTransform::FAnimNode_CustomizableBoneTransform() : FAnimNode_SkeletalControlBase()
{

}


void FAnimNode_CustomizableBoneTransform::GatherDebugData(FNodeDebugData& DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)
	FString DebugLine = DebugData.GetNodeName(this);

	DebugLine += "(";
	this->AddDebugNodeData(DebugLine);
	DebugLine += FString::Printf(TEXT(" Target: %s)"), *FString("Multiple Bones"));
	DebugData.AddDebugItem(DebugLine);

	this->ComponentPose.GatherDebugData(DebugData);
}

void FAnimNode_CustomizableBoneTransform::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	
	if (auto CustomizableInst = Cast<ICustomizableAnimInstanceInterface>(Context.GetAnimInstanceObject()))
	{
		for (const auto& Data : this->BonesToModify)
		{
			FName Key = Data.Key;
			CustomizableInst->RegisterFloatValue(
				Key,
				FCustomizableFloatListener::CreateLambda([Key, this](float NewAlpha)
					{
						this->BonesToModify[Key].Alpha = NewAlpha;
					}));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Attempted to use customizable node in non-customizable anim instance."));
	}
	

	Super::Initialize_AnyThread(Context);
}

void FAnimNode_CustomizableBoneTransform::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(EvaluateSkeletalControl_AnyThread)

	check(OutBoneTransforms.Num() == 0);

	for (const auto& Points : this->BonesToModify)
	{
		for (const auto& Point : Points.Value.Points)
		{
			this->HandleIndividualBoneCase(Points.Value.Alpha, Point, Output, OutBoneTransforms);
		}
	}
}
void FAnimNode_CustomizableBoneTransform::ForEachBone(const FPerDataPointDelegate& Callback) const
{
	if (Callback.IsBound())
	{
		for (const auto& SetPoint : this->BonesToModify)
		{
			for (const auto& Point : SetPoint.Value.Points)
			{
				Callback.Execute(Point);
			}
		}
	}
}

bool FAnimNode_CustomizableBoneTransform::IsEmpty() const
{
	if (this->BonesToModify.IsEmpty())
	{
		return true;
	}
	else
	{
		for (const auto& Pair : this->BonesToModify)
		{
			if (Pair.Value.Points.IsEmpty())
			{
				return true;
			}
		}

		return false;
	}
}

void FAnimNode_CustomizableBoneTransform::SetData(const FAnimNode_CustomizableBoneTransform& Other)
{
	this->BonesToModify.Empty();
	this->BonesToModify.Append(Other.BonesToModify);
}


bool FAnimNode_CustomizableBoneTransform::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	if (this->IsEmpty())
	{
		return false;
	}
	else
	{
		for (const auto& Points : this->BonesToModify)
		{
			if (Points.Value.Points.IsEmpty())
			{
				return false;
			}

			for (const auto& Point : Points.Value.Points)
			{
				if (!Point.BoneToModify.IsValidToEvaluate(RequiredBones))
				{
					return false;
				}
			}
		}

		return true;
	}
}

void FAnimNode_CustomizableBoneTransform::HandleIndividualBoneCase(
	float SetAlpha,
	const FAnimNode_CustomizableBoneTransformDataPoint& DataPoint,
	FComponentSpacePoseContext& Output,
	TArray<FBoneTransform>& OutBoneTransforms)
{
	ANIM_MT_SCOPE_CYCLE_COUNTER_VERBOSE(BoneTransformCustomizer, !IsInGameThread());
	const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();

	FCompactPoseBoneIndex CompactPoseBoneToModify = DataPoint.BoneToModify.GetCompactPoseIndex(BoneContainer);
	FTransform NewBoneTM = Output.Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
	FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();

	if (DataPoint.ScaleMode != BMM_Ignore)
	{
		// Convert to Bone Space.
		FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, DataPoint.ScaleSpace);

		FVector Scale = DataPoint.Scale.GetValue(SetAlpha);

		if (DataPoint.ScaleMode == BMM_Additive)
		{
			NewBoneTM.SetScale3D(NewBoneTM.GetScale3D() * Scale);
		}
		else
		{
			NewBoneTM.SetScale3D(Scale);
		}

		// Convert back to Component Space.
		FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, DataPoint.ScaleSpace);
	}

	if (DataPoint.RotationMode != BMM_Ignore)
	{
		// Convert to Bone Space.
		FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, DataPoint.RotationSpace);

		FVector RotVector = DataPoint.Rotation.GetValue(SetAlpha);
		FRotator Rotator(RotVector.Y, RotVector.Z, RotVector.X);
		const FQuat BoneQuat(Rotator);

		if (DataPoint.RotationMode == BMM_Additive)
		{
			NewBoneTM.SetRotation(BoneQuat * NewBoneTM.GetRotation());
		}
		else
		{
			NewBoneTM.SetRotation(BoneQuat);
		}

		// Convert back to Component Space.
		FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, DataPoint.RotationSpace);
	}

	if (DataPoint.TranslationMode != BMM_Ignore)
	{
		// Convert to Bone Space.
		FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, DataPoint.TranslationSpace);
		FVector Translation = DataPoint.Translation.GetValue(SetAlpha);

		if (DataPoint.TranslationMode == BMM_Additive)
		{
			NewBoneTM.AddToTranslation(Translation);
		}
		else
		{
			NewBoneTM.SetTranslation(Translation);
		}

		// Convert back to Component Space.
		FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, DataPoint.TranslationSpace);
	}

	OutBoneTransforms.Add(FBoneTransform(DataPoint.BoneToModify.GetCompactPoseIndex(BoneContainer), NewBoneTM));

	TRACE_ANIM_NODE_VALUE(Output, TEXT("Target"), DataPoint.BoneToModify.BoneName);
}

void FAnimNode_CustomizableBoneTransform::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(InitializeBoneReferences)

	for (auto& Points : this->BonesToModify)
	{
		for (auto& Point : Points.Value.Points)
		{
			Point.BoneToModify.Initialize(RequiredBones);
		}
	}
}
