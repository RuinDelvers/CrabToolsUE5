#pragma once

#include "StateMachine/AI/SimpleMoveTo.h"
#include "MoveToInteract.generated.h"

class UAIInteractionData;

/**
 * Simple node for making an entity move to a given actor.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAIMoveToInteractNode : public UAISimpleMoveToNode
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<UAIInteractionData> CurrentData;

public:

	UAIMoveToInteractNode();

	virtual void Initialize_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;

private:
	
	UInteractableComponent* GetInteractable() const;

	void BindEvents();

	UFUNCTION()
	void OnInteractableAdded(UInteractableComponent* Interactable);

	void ComputeTarget();

	class UInteractionSystem* GetInteractionComponent() const;

	bool HandleInteraction();
	bool HasInteractable() const;
};
