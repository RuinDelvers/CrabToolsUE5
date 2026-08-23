#pragma once

#include "StateMachine/StateMachine.h"
#include "BlockMouseOverNode.generated.h"

class UMouseOverComponent;

/**
 * Node used to add and remove contexts for the enhanced input system.
 */
UCLASS(Blueprintable, EditInlineNew, DontCollapseCategories, Category = "StateMachine|Input")
class CRABTOOLSUE5_API UBlockMouseOverNode : public UStateNode
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, Category = "MouseOver")
	TObjectPtr<UMouseOverComponent> MouseOver;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MouseOver")
	bool bInvert = false;

public:


protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
};