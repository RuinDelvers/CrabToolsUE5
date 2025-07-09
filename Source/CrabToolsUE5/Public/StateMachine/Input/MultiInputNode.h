#pragma once

#include "StateMachine/StateMachine.h"
#include "GameFramework/Pawn.h"
#include "MultiInputNode.generated.h"

/**
 * This node is used to bind as a subobject for a pawn's input. When used as node for a Pawn's
 * State Machine, it'll search for its input component and bind this object to its delegates.
 * This only will have an effect for BP classes, as those can have the EnhancedInput nodes
 * in their event graph to be searched for.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DontCollapseCategories, Category = "StateMachine|Input")
class CRABTOOLSUE5_API UMultiInputNode : public UStateNode
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, Category="Pawn")
	TObjectPtr<APawn> PawnOwner;

public:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
};