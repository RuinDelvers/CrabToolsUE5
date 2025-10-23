#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "AbilityNode.generated.h"

class UAbility;

/* 
 * Node which handles an ability performing its task and monitoring it's finish. The
 * Data passed to this node can either be a node itself, or something which implements
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAbilityNode : public UStateNode
{
	GENERATED_BODY()

	/* The default ability to perform when entering this state. */
	UPROPERTY(BlueprintReadOnly, Instanced, Category = "Ability", meta=(AllowPrivateAccess))
	TObjectPtr<UAbility> DefaultAbility;

	/* The ability to start when entering this node. */
	TObjectPtr<UAbility> Selected;

	/* Flag used to control immediate ability finish checking. */
	bool bNeedsFinishing = false;

public:

	UAbilityNode();

	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;
	virtual bool RequiresTick_Implementation() const override;

private:
	UFUNCTION()
	void HandleFinish(UAbility* Abi);
};
