#pragma once

#include "StateMachine/StateMachine.h"
#include "StateMachine/General/NodeWithChild.h"
#include "SequenceWaitUntilPauseNode.generated.h"

/* 
 *
 */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API USequenceWaitUntilPauseNode : public UNodeWithChild
{
	GENERATED_BODY()


public:

	USequenceWaitUntilPauseNode();

protected:

	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

private:

	UFUNCTION()
	void OnPaused();
};
