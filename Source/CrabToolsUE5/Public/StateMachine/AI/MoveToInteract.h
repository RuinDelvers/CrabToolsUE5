#pragma once

#include "StateMachine/AI/SimpleMoveTo.h"
#include "MoveToInteract.generated.h"

/**
 * Base interaction class; Contains the interactor only. Extend this to hold data for
 * specific interactions for interactables.
 */
UCLASS(Blueprintable, Category = "Interaction")
class CRABTOOLSUE5_API UAIInteractionData : public UObject
{
	GENERATED_BODY()

public:

	/* Convenience function for quickly makine of these object in a pure manner. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	static UAIInteractionData* MakeInteractionData(
		FName InitInteraction,
		AActor* InitInteractable,
		UObject* InitData);

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ExposeOnSpawn = true))
	FName Interaction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ExposeOnSpawn = true))
	TObjectPtr<AActor> Interactable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ExposeOnSpawn = true))
	TObjectPtr<UObject> Data;

public:

	bool IsValidData() const;
};

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
	virtual void PostTransition_Inner_Implementation() override;
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
