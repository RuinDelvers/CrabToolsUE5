#pragma once

#include "StateMachine/StateMachine.h"
#include "LevelSequencePlayer.h"
#include "SequenceOriginSetNode.generated.h"

UENUM()
enum class ETransformOriginSource
{
	INLINE UMETA(DisplayName="Inline"),
	USE_ACTOR_SELF UMETA(DisplayName = "Self"),
	USE_TRANSFORM_SELF UMETA(DisplayName = "Self"),
};

/* This node sets the dynamic origin of the sequence. */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API USequenceOriginSetNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data",
		meta=(AllowPrivateAccess))
	ETransformOriginSource Source = ETransformOriginSource::USE_TRANSFORM_SELF;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data",
		meta = (AllowPrivateAccess,
			EditCondition="Source==ETransformOriginSource::INLINE",
			EditConditionHides))
	FTransform InlineTransform;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
};
