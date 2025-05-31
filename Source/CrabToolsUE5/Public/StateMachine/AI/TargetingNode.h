#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "TargetingNode.generated.h"


/**
 * Node for managing a targeting phase of using an ability.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UTargetingNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UObject> TargetingInterface;

public:

	UTargetingNode();

protected:

	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual bool HasPipedData_Implementation() const override;
	virtual UObject* GetPipedData_Implementation() override;

private:

	UFUNCTION()
	void OnConfirmed(TScriptInterface<ITargetingControllerInterface> Targeter);
};
