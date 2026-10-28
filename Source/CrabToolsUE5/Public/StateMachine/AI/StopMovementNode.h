#pragma once

#include "StateMachine/AI/BaseNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/AI/AIStructs.h"
#include "StopMovementNode.generated.h"

/**
 * Simple node for stopping movement on an AI controller.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UStopMovementNode : public UAIBaseNode
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="Phase")
	bool bOnEnter = true;

	UPROPERTY(EditAnywhere, Category = "Phase")
	bool bOnExit = false;

protected:

	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

};
