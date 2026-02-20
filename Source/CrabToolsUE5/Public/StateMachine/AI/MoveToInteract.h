#pragma once

#include "StateMachine/AI/SimpleMoveTo.h"
#include "MoveToInteract.generated.h"

class UAIInteractionData;
class UInteractionComponent;

/**
 * Simple node for making an entity move to a given actor. Also functions as a MoveTo node.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAIMoveToInteractNode : public UAISimpleMoveToNode
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<UInteractionComponent> Component;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction", meta=(AllowPrivateAccess))
	TObjectPtr<UAIInteractionData> CurrentData;

public:

	UAIMoveToInteractNode();

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	//virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void EventWithData_Inner_Implementation(FName InEvent, UObject* Data, UObject* EventSource) override;
	virtual void OnMoveCompleted_Implementation(FAIRequestID RequestID, EPathFollowingResult::Type MoveResult);

	UAIInteractionData* GetCurrentInteractionData() const { return this->CurrentData; }

private:
	
	UInteractableComponent* GetInteractable() const;

	void BindEvents();
	void UnbindEvents();

	UFUNCTION()
	void OnInteractableAdded(UInteractableComponent* Interactable);

	//void ComputeTarget();

	bool HandleInteraction();
	bool HasInteractable() const;

	void InteractWithRequest(UAIInteractionData* Interaction);
	void RouteRequest(UObject* InData);
};
