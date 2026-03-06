#include "Animation/Nodes/AnimNode_BoneTransformCustomizer.h"
#include "Animation/AnimInstanceProxy.h"

FAnimNode_BoneTransformCustomizer::FAnimNode_BoneTransformCustomizer()
{
	
}

void FAnimNode_BoneTransformCustomizer::GatherDebugData(FNodeDebugData& DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)
	FString DebugLine = DebugData.GetNodeName(this);

	DebugLine += "(";
	this->AddDebugNodeData(DebugLine);
	DebugLine += FString::Printf(TEXT(" Target: %s)"), *FString("Multiple Bones"));
	DebugData.AddDebugItem(DebugLine);

	this->ComponentPose.GatherDebugData(DebugData);
}

void FAnimNode_BoneTransformCustomizer::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(EvaluateSkeletalControl_AnyThread)
	
	check(OutBoneTransforms.Num() == 0);

	for (const auto& Points : this->BonesToModify)
	{
		for (const auto& Point : Points.Value.Points)
		{
			this->HandleIndividualBoneCase(Point, Output, OutBoneTransforms);
		}
	}
}

bool FAnimNode_BoneTransformCustomizer::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	for (const auto& Points : this->BonesToModify)
	{
		for (const auto& Point : Points.Value.Points)
		{
			if (!Point.BoneToModify.IsValidToEvaluate(RequiredBones))
			{
				return false;
			}
		}
	}

	return !this->BonesToModify.IsEmpty();
}

void FAnimNode_BoneTransformCustomizer::ForEachBone(const FPerDataPointDelegate& Callback) const
{
	if (Callback.IsBound())
	{
		for (const auto& Set : this->BonesToModify)
		{
			for (const auto& Point : Set.Value.Points)
			{
				Callback.Execute(Point);
			}
		}
	}
}

bool FAnimNode_BoneTransformCustomizer::IsEmpty() const
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

void FAnimNode_BoneTransformCustomizer::SetData(const FAnimNode_BoneTransformCustomizer& Other)
{
	this->BonesToModify.Empty();
	this->BonesToModify.Append(Other.BonesToModify);

	UE_LOG(LogTemp, Warning, TEXT("- After applied data in SetData"));
	for (const auto& Data : this->BonesToModify)
	{
		for (const auto& Point : Data.Value.Points)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bone: %s"), *Point.BoneToModify.BoneName.ToString());
		}
	}
}

FAnimNode_BoneTransformCustomizer& FAnimNode_BoneTransformCustomizer::operator=(const FAnimNode_BoneTransformCustomizer& Other)
{
	this->BonesToModify.Empty();
	this->BonesToModify.Append(Other.BonesToModify);

	return *this;
}

void FAnimNode_BoneTransformCustomizer::InitializeBoneReferences(const FBoneContainer& RequiredBones)
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

void FAnimNode_BoneTransformCustomizer::HandleIndividualBoneCase(
	const FAnimNode_BoneTransformCustomizerDataPoint& DataPoint,
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

		FVector Scale = DataPoint.Scale.GetValue(DataPoint.Alpha);

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
		
		FVector RotVector = DataPoint.Rotation.GetValue(DataPoint.Alpha);
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
		FVector Translation = DataPoint.Translation.GetValue(DataPoint.Alpha);

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
