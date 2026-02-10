#pragma once

#include "StateMachine/RPG/RPGStateNode.h"
#include "RPGTurnStartedNode.generated.h"

class URPGComponent;

/**
 * Node that listens to the turn Started event for the owner's RPG component, and emits an event when
 * that happens.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API URPGTurnStartedNode : public URPGStateNode
{
	GENERATED_BODY()

public:

	URPGTurnStartedNode();

protected:

	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

private:

	UFUNCTION()
	void TurnStarted(URPGComponent* Comp);
};