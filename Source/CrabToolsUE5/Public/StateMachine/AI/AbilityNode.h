#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/StateMachineEnum.h"
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
	UPROPERTY(VisibleAnywhere, Category="Ability")
	TObjectPtr<UAbility> Selected;

	/* Flag used to control immediate ability finish checking. */
	bool bNeedsFinishing = false;

	/* Derived data for this is the targeting controller. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess))
	EStateMachineGenericPipedDataProcedure PipedDateProcedure = EStateMachineGenericPipedDataProcedure::DERIVED;

	UPROPERTY()
	TObjectPtr<UObject> CachedPipedData;

public:

	UAbilityNode();

	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;
	virtual bool RequiresTick_Implementation() const override;

	virtual bool HasPipedData_Implementation() const override;
	virtual UObject* GetPipedData_Implementation() override;

protected:

	void StartAbility(UAbility* Abi, UObject* StartData);

private:
	UFUNCTION()
	void HandleFinish(UAbility* Abi);
};
