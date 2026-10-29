#pragma once

#include "StateMachine/StateMachine.h"
#include "GameFramework/Pawn.h"
#include "BaseInputNode.generated.h"

class UInputAction;

/**
 * This input node will map InputActions to events to be emitted.
 */
UCLASS(Blueprintable, EditInlineNew, DontCollapseCategories, Category = "StateMachine|Input")
class CRABTOOLSUE5_API UBaseInputNode : public UStateNode
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Input")
	TObjectPtr<APawn> PawnOwner;

	UPROPERTY(BlueprintReadOnly, Category = "Input")
	TObjectPtr<UEnhancedInputComponent> Component;

protected:

	virtual void Initialize_Inner_Implementation() override;
};