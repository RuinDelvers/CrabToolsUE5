#pragma once

#include "StateMachine/StateMachine.h"
#include "LevelSequencePlayer.h"
#include "SequenceTimeStepNode.generated.h"


/*
 * This node sets the time for the current playing sequence if it is
 * the given map. Can set exact times, marks, or otherwise.
 */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API USequenceTimeStepNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> Player;

	UPROPERTY(EditAnywhere, Category = "Sequence")
	bool bEnterStep = true;

	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess, EditCondition="bEnterStep", InlineEditConditionToggle))
	FMovieSceneSequencePlaybackParams EnterSteps;

	UPROPERTY(EditAnywhere, Category = "Sequence")
	bool bExitStep = true;

	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess, EditCondition = "bExitStep", InlineEditConditionToggle))
	FMovieSceneSequencePlaybackParams ExitSteps;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

private:
};
