#pragma once

#include "StateMachine/StateMachine.h"
#include "Sequencer/SequencerStruct.h"
#include "SequenceActionNode.generated.h"


UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API USequenceActionNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> Player;

	UPROPERTY(EditAnywhere, Category="Pausing")
	FSequencerAction EnterAction;

	UPROPERTY(EditAnywhere, Category = "Pausing")
	FSequencerAction ExitAction;


protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

private:

};
