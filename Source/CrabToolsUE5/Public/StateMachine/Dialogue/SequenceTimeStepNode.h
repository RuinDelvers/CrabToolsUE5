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

	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess))
	FMovieSceneSequencePlaybackParams EnterSteps;

	UPROPERTY(EditAnywhere, Category = "Sequence",
		meta = (AllowPrivateAccess))
	FMovieSceneSequencePlaybackParams ExitSteps;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
};
