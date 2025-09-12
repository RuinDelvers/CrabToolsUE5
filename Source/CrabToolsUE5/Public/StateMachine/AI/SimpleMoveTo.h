#pragma once

#include "Properties/GenericPropertyBinding.h"
#include "StateMachine/AI/BaseNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/AI/AIStructs.h"
#include "SimpleMoveTo.generated.h"

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

protected:

	UPROPERTY(EditDefaultsOnly, Category="AI", meta=(AllowPrivateAccess,
		ShowOnlyInnerProperties))
	FMoveToData MoveData;

	FPathFollowingRequestResult Result;

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

	UFUNCTION()
	void EventNotify_PauseMovement(FName EName);

	UFUNCTION()
	void EventNotify_ResumeMovement(FName EName);

private:

	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MoveResult);

	void BindCallback();
	void UnbindCallback();

	void MoveTo();
};
