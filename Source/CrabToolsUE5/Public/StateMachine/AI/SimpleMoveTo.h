#pragma once

#include "Properties/GenericPropertyBinding.h"
#include "StateMachine/AI/BaseNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/AI/AIStructs.h"
#include "SimpleMoveTo.generated.h"

/**
 * Simple node for making an a pawn or AI Controller 
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

public:

	UAISimpleMoveToNode();

	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void SetActive_Inner_Implementation(bool bNewActive) override;

protected:

	FORCEINLINE EPathFollowingResult::Type GetMovementResult() const { return this->MovementResult; }

	void StopMovement();
	void SetOverrideLocation(FVector Location);

	UFUNCTION()
	void EventNotify_PauseMovement(FName InEvent);

	UFUNCTION()
	void EventNotify_ResumeMovement(FName InEvent);

private:

	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MoveResult);

	void BindCallback();
	void UnbindCallback();

	void MoveTo();
};
