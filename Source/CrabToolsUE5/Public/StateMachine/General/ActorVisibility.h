#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "ActorVisibility.generated.h"

/**
 * Node that either hides or shows an actor.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UActorVisibilityNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "AI", meta = (ShowInnerProperties))
	TObjectPtr<UStateMachineProperty> Property;

	/* Whether to show or hide the actor on entering this node. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess))
	bool bShowActor = false;

	/* Whether or not the revert to the previous visibility on exit. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess))
	bool bRevertOnExit = true;

public:

	UActorVisibilityNode();

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;	

private:

	AActor* GetActor() const;
};
