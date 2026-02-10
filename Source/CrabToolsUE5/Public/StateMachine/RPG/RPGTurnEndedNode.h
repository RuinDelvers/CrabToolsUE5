#pragma once

#include "StateMachine/RPG/RPGStateNode.h"
#include "RPGTurnEndedNode.generated.h"

class URPGComponent;

/**
 * Node that listens to the turn end event for the owner's RPG component, and emits an event when
 * that happens.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API URPGTurnEndedNode : public URPGStateNode
{
	GENERATED_BODY()

public:

	URPGTurnEndedNode();

protected:

	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

private:

	UFUNCTION()
	void TurnEnded(URPGComponent* Comp);
};