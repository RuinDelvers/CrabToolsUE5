#pragma once

#include "UObject/ObjectMacros.h"
#include "BoneContainer.h"
#include "BonePose.h"
#include "BoneControllers/AnimNode_ModifyBone.h"
#include "Curves/CurveVector.h"
#include "AnimNode_BoneTransformCustomizer.generated.h"

class USkeletalMeshComponent;

USTRUCT(BlueprintInternalUseOnly)
struct FAnimNode_BoneTransformCustomizerDataPoint
{
	GENERATED_BODY()

	/* The  bone to modify. */
	UPROPERTY(EditAnywhere, Category = SkeletalControl)
	FBoneReference BoneToModify;

	/* The current sample along the curve. */
	UPROPERTY(EditAnywhere, Category = SkeletalControl)
	float Alpha = 0.5;

	UPROPERTY(EditAnywhere, Category = Transform)
	TEnumAsByte<EBoneModificationMode> TranslationMode = BMM_Ignore;

	UPROPERTY(EditAnywhere, Category = Transform, meta = (EditCondition = "TranslationMode!=EBoneModificationMode::BMM_Ignore", EditConditionHides))
	TEnumAsByte<enum EBoneControlSpace> TranslationSpace = BCS_ComponentSpace;

	UPROPERTY(EditAnywhere, Category = Transform, meta=(EditCondition="TranslationMode!=EBoneModificationMode::BMM_Ignore", EditConditionHides))
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
struct FAnimNode_BoneTransformCustomizerDataSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = SkeletalControl)
	TArray<FAnimNode_BoneTransformCustomizerDataPoint> Points;
};

/**
 *	Node that allows transformation of bones
 */
USTRUCT(BlueprintInternalUseOnly)
struct CRABTOOLSUE5_API FAnimNode_BoneTransformCustomizer : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

private:


	UPROPERTY(EditAnywhere, Category="SkeletalControl")
	TMap<FName, FAnimNode_BoneTransformCustomizerDataSet> BonesToModify;

public:

	DECLARE_DELEGATE_OneParam(FPerDataPointDelegate, const FAnimNode_BoneTransformCustomizerDataPoint&);

public:

	FAnimNode_BoneTransformCustomizer();

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	void ForEachBone(const FPerDataPointDelegate& Callback) const;
	bool IsEmpty() const;

	void SetData(const FAnimNode_BoneTransformCustomizer& Other);
	
	FAnimNode_BoneTransformCustomizer& operator=(const FAnimNode_BoneTransformCustomizer& Other);

private:
	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	void HandleIndividualBoneCase(
		const FAnimNode_BoneTransformCustomizerDataPoint& DataPoint,
		FComponentSpacePoseContext& Output,
		TArray<FBoneTransform>& OutBoneTransforms);
};
