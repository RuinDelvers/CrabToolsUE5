#pragma once

#include "Properties/GenericPropertyBinding.h"
#include "StateMachine/AI/BaseNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateMachine/AI/AIStructs.h"
#include "SimpleMoveTo.generated.h"

/**
 * Simple node for making an a pawn or AI Controller move to a location or actor.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAISimpleMoveToNode : public UAIBaseNode
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, Category = "Binding", meta = (ShowInnerProperties))
	TObjectPtr<UGenericPropertyBinding> Property;

protected:

	UPROPERTY(EditDefaultsOnly, Category="AI", meta=(AllowPrivateAccess,
		ShowOnlyInnerProperties))
	FMoveToData MoveData;

public:

	UAISimpleMoveToNode();

	virtual void EventWithData_Inner_Implementation(FName InEvent, UObject* Data, UObject* Source) override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void SetActive_Inner_Implementation(bool bNewActive) override;

protected:

	void StopMovement();

	UFUNCTION()
	void EventNotify_PauseMovement(FName InEvent, UObject* EventSource);

	UFUNCTION()
	void EventNotify_ResumeMovement(FName InEvent, UObject* EventSource);

	/*
	 * Call this to begin movement to a preset location. If the property binding is bound, then it will be used.
	 * Otherwise, it will attempt to make a request to move to a specific location, if the flag is set in MoveToData.
	 */
	void MoveToPreset();

	/*
	 * Call this to move according to a request implement the MovementRequestInterface.
	 */
	void MoveToRequest(UObject* Request);

	UFUNCTION(BlueprintNativeEvent, Category="Movement")
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MoveResult);
	virtual void OnMoveCompleted_Implementation(FAIRequestID RequestID, EPathFollowingResult::Type MoveResult);

	/* This is called prior to setting up a new movement request. Useful for setting up listeners for OnMoveCompleted. */
	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	void OnRequestStarted();
	virtual void OnRequestStarted_Implementation() {}

private:
	
	void BindCallback();
	void UnbindCallback();
};
