#pragma once

#include "Properties/GenericPropertyBinding.h"
#include "StateMachine/StateMachine.h"
#include "SetBooleanNode.generated.h"


/**
 * Sets a boolean value in a state machine.
 */
UCLASS(Blueprintable, Category = "StateMachine|Properties")
class CRABTOOLSUE5_API USetBooleanNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, Category = "Properties", meta = (ShowInnerProperties))
	TObjectPtr<UGenericPropertyBinding> Property;

	UPROPERTY(EditAnywhere, Category = "Properties", meta = (AllowPrivateAccess))
	bool bOnEnter = true;

	UPROPERTY(EditAnywhere, Category = "Properties", meta = (AllowPrivateAccess))
	bool bOnExit = false;

public:

	USetBooleanNode();

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
};
