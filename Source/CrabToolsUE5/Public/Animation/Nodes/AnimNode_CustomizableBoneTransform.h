#pragma once

#include "UObject/ObjectMacros.h"
#include "BoneContainer.h"
#include "BonePose.h"
#include "BoneControllers/AnimNode_ModifyBone.h"
#include "Curves/CurveVector.h"
#include "AnimNode_CustomizableBoneTransform.generated.h"

class USkeletalMeshComponent;

USTRUCT(BlueprintInternalUseOnly)
struct FAnimNode_CustomizableBoneTransformDataPoint
{
	GENERATED_BODY()

	/* The  bone to modify. */
	UPROPERTY(EditAnywhere, Category = SkeletalControl)
	FBoneReference BoneToModify;

	UPROPERTY(EditAnywhere, Category = Transform)
	TEnumAsByte<EBoneModificationMode> TranslationMode = BMM_Ignore;

	UPROPERTY(EditAnywhere, Category = Transform, meta = (EditCondition = "TranslationMode!=EBoneModificationMode::BMM_Ignore", EditConditionHides))
	TEnumAsByte<enum EBoneControlSpace> TranslationSpace = BCS_ComponentSpace;

	UPROPERTY(EditAnywhere, Category = Transform, meta = (EditCondition = "TranslationMode!=EBoneModificationMode::BMM_Ignore", EditConditionHides))
	FRuntimeVectorCurve Translation;

	UPROPERTY(EditAnywhere, Category = Rotation)
	TEnumAsByte<EBoneModificationMode> RotationMode = BMM_Ignore;

	UPROPERTY(EditAnywhere, Category = Rotation, meta = (EditCondition = "RotationMode != EBoneModificationMode::BMM_Ignore", EditConditionHides))
	TEnumAsByte<enum EBoneControlSpace> RotationSpace = BCS_ComponentSpace;

	UPROPERTY(EditAnywhere, Category = Rotation, meta = (EditCondition = "RotationMode != EBoneModificationMode::BMM_Ignore", EditConditionHides))
	FRuntimeVectorCurve Rotation;

	UPROPERTY(EditAnywhere, Category = Scale)
	TEnumAsByte<EBoneModificationMode> ScaleMode = BMM_Ignore;

	UPROPERTY(EditAnywhere, Category = Scale, meta = (EditCondition = "ScaleMode != EBoneModificationMode::BMM_Ignore", EditConditionHides))
	TEnumAsByte<enum EBoneControlSpace> ScaleSpace = BCS_ComponentSpace;

	UPROPERTY(EditAnywhere, Category = Scale, meta = (EditCondition = "ScaleMode != EBoneModificationMode::BMM_Ignore", EditConditionHides))
	FRuntimeVectorCurve Scale;
};

USTRUCT(BlueprintInternalUseOnly)
struct FAnimNode_CustomizableBoneTransformDataSet
{
	GENERATED_BODY()

	/* The current sample along the curve. */
	UPROPERTY(EditAnywhere, Category = SkeletalControl)
	float Alpha = 0.5;

	UPROPERTY(EditAnywhere, Category = SkeletalControl)
	TArray<FAnimNode_CustomizableBoneTransformDataPoint> Points;
};

/**
 *	Node that allows transformation of bones
 */
USTRUCT(BlueprintInternalUseOnly)
struct CRABTOOLSUE5_API FAnimNode_CustomizableBoneTransform : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "SkeletalControl")
	TMap<FName, FAnimNode_CustomizableBoneTransformDataSet> BonesToModify;

public:

	DECLARE_DELEGATE_OneParam(FPerDataPointDelegate, const FAnimNode_CustomizableBoneTransformDataPoint&);

public:

	FAnimNode_CustomizableBoneTransform();

	void SetData(const FAnimNode_CustomizableBoneTransform& Other);
	bool IsEmpty() const;
	void ForEachBone(const FPerDataPointDelegate& Callback) const;

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface
	
protected:

	void HandleIndividualBoneCase(
		float SetAlpha,
		const FAnimNode_CustomizableBoneTransformDataPoint& DataPoint,
		FComponentSpacePoseContext& Output,
		TArray<FBoneTransform>& OutBoneTransforms);

private:


	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface
};
