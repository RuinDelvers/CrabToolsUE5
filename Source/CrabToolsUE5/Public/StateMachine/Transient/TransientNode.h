#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "TransientNode.generated.h"

class UTransientTask;

/**
 * State Machine Node that is a composite of other nodes.
 */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine")
class CRABTOOLSUE5_API UTransientNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<UTransientTask> CurrentTask;
	bool bHasFinishedTask = false;

public:

	UTransientNode();

	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;	
	virtual void Enter_Inner_Implementation() override;
	virtual bool RequiresTick_Implementation() const override;

private:

	UFUNCTION()
	void OnTaskFinished();
};
