#pragma once

#include "Properties/GenericPropertyBinding.h"
#include "StateMachine/AI/BaseNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "SimpleMoveTo.generated.h"

struct FMoveToData;

/**
 * Simple node for making an entity move to a given actor.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAISimpleMoveToNode : public UAIBaseNode
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, Category = "AI", meta = (ShowInnerProperties))
	TObjectPtr<UGenericPropertyBinding> Property;

	EPathFollowingResult::Type MovementResult;

	/* Overriden location for extended nodes to use. This is used on a per Enter basis.*/
	FVector OverrideLocation;
	bool bUseOverrideLocation = false;

protected:

	UPROPERTY(VisibleAnywhere, Transient, Category="AI")
	TObjectPtr<AActor> GoalActor;

public:

	UAISimpleMoveToNode();

	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Initialize_Inner_Implementation() override;
	virtual void PostTransition_Inner_Implementation() override;

	#if WITH_EDITOR
		virtual void PostLinkerChange() override;
	#endif

protected:

	FORCEINLINE EPathFollowingResult::Type GetMovementResult() const { return this->MovementResult; }

	void StopMovement();
	void SetOverrideLocation(FVector Location);

private:

	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	void BindCallback();
	void UnbindCallback();

	void MoveTo();
};
