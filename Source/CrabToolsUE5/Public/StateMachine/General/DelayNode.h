#pragma once

#include "StateMachine/General/NodeWithChild.h"
#include "DelayNode.generated.h"

/**
 * This node delays the entry for its child node.
 */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine")
class CRABTOOLSUE5_API UDelayNode : public UNodeWithChild
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nodes",
		meta = (AllowPrivateAccess))
	float DelayTime = 0.0f;

	FTimerHandle Timer;

public:

	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	
private:

	UFUNCTION()
	void HandleEnter();

};
