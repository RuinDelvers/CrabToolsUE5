#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "CallDelegateNode.generated.h"


/**
 * Sets a boolean value in a state machine.
 */
UCLASS(Blueprintable, Category = "StateMachine|Properties")
class CRABTOOLSUE5_API UCallDelegateNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, Category = "AI", meta = (ShowInnerProperties))
	TObjectPtr<UStateMachineProperty> EnterProperty;

	UPROPERTY(VisibleAnywhere, Category = "AI", meta = (ShowInnerProperties))
	TObjectPtr<UStateMachineProperty> ExitProperty;

public:

	UCallDelegateNode();

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

private:
	UFUNCTION()
	void DelegateSignatureFunction() {}
};
