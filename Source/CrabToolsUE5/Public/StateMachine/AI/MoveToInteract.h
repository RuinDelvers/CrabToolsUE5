#pragma once

#include "StateMachine/AI/SimpleMoveTo.h"
#include "MoveToInteract.generated.h"

class UAIInteractionData;

/**
 * Simple node for making an entity move to a given actor. Also functions as a MoveTo node.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAIMoveToInteractNode : public UAISimpleMoveToNode
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<UAIInteractionData> CurrentData;

	/*
	 * If true, the most recent interactio ndata will not be nulled upon exiting. It will be nulled upon completing
	 * the interaction, however.
	 */
	UPROPERTY(EditAnywhere, Category="Interaction")
	bool bCacheInteractionData = false;

public:

	UAIMoveToInteractNode();

	virtual void Initialize_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void EventWithData_Inner_Implementation(FName InEvent, UObject* Data, UObject* EventSource) override;

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
