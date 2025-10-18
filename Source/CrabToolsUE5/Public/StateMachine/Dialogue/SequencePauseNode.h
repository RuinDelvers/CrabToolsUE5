#pragma once

#include "StateMachine/StateMachine.h"
#include "LevelSequencePlayer.h"
#include "SequencePauseNode.generated.h"

UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API USequencePauseNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> Player;

	/* Whether or not to set the pause flag when entering this node. */
	UPROPERTY(EditAnywhere, Category="Pausing")
	bool bPauseOnEnter = true;

	UPROPERTY(EditAnywhere, Category = "Pausing",
		meta=(EditCondition="bPauseOnEnter", InlineEditConditionToggle))
	bool bIsPausedOnEnter = true;

	/* Whether or not to set the pause flag when exiting this node. */
	UPROPERTY(EditAnywhere, Category = "Pausing")
	bool bPauseOnExit = true;

	UPROPERTY(EditAnywhere, Category = "Pausing",
		meta = (EditCondition = "bPauseOnExit", InlineEditConditionToggle))
	bool bIsPausedOnExit = false;


protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
};
