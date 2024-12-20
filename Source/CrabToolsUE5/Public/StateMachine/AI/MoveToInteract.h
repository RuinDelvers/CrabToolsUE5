#pragma once

#include "CoreMinimal.h"
#include "StateMachine/AI/SimpleMoveTo.h"
#include "Navigation/PathFollowingComponent.h"
#include "MoveToInteract.generated.h"

/**
 * Simple node for making an entity move to a given actor.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAIMoveToInteractNode : public UAISimpleMoveToNode
{
	GENERATED_BODY()

public:

	UAIMoveToInteractNode();

	virtual void Initialize_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	virtual void PostTransition_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;

private:
	
	void BindEvents();

	UFUNCTION()
	void OnInteractableAdded(TScriptInterface<IInteractableInterface> Interactable);

	void ComputeTarget();

	class UInteractionSystem* GetInteractionComponent() const;

	bool HandleInteraction();
	bool HasInteractable() const;
};
