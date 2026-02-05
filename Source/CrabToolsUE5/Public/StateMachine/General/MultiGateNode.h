#pragma once

#include "StateMachine/StateMachine.h"
#include "MultiGateNode.generated.h"

UCLASS(Abstract, EditInlineNew, DefaultToInstanced, Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UMultiGateNodeGate : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY(BlueprintReadOnly, Category="StateMachine", meta=(AllowPrivateAccess))
	TObjectPtr<UStateMachine> Machine;

public:

	DECLARE_MULTICAST_DELEGATE(FGateStateChanged);

	FGateStateChanged OnGateStateChanged;

public:

	UFUNCTION(BlueprintCallable, Category="Update")
	void UpdateGate() const;

	void Initialize(UStateMachine* InitMachine);

	/* Returns whether or not this gate should be active or not. */
	UFUNCTION(BlueprintNativeEvent, Category="Gate")
	bool GetGateActivity() const;
	virtual bool GetGateActivity_Implementation() const { return false; }

protected:

	UFUNCTION(BlueprintNativeEvent, Category="Gate", meta=(DisplayName="Initialize"))
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation() {}
};

USTRUCT(BlueprintType)
struct FMultiGateNodeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, Category="Nodes")
	TObjectPtr<UStateNode> Node;

	UPROPERTY(EditAnywhere, Instanced, Category = "Gate")
	TObjectPtr<UMultiGateNodeGate> Gate;
};

/**
 * This node is a gate node controll node activeness. It maps nodes to a corresponding
 * control gate that it listens to. When the gate changes it state, it will deactivate the node.
 * 
 * These gates still permit Enter and Exit calls from happening regardless of a gate's state.
 */
UCLASS(BlueprintType, Category = "StateMachine")
class CRABTOOLSUE5_API UMultiGateNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category="Gates", meta=(AllowPrivateAccess))
	TArray<FMultiGateNodeData> Nodes;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual bool Verify_Inner(FNodeVerificationContext& Context) const override;
	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Event_Inner_Implementation(FName Event) override;
	virtual void EventWithData_Inner_Implementation(FName InEvent, UObject* Data) override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;
	virtual void ExitWithData_Inner_Implementation(UObject* Data) override;
	virtual bool RequiresTick_Implementation() const override;
	virtual bool HasPipedData_Implementation() const override;
	virtual UObject* GetPipedData_Implementation() override;
	virtual void SetActive_Inner_Implementation(bool bNewActive) override;

private:

	UFUNCTION()
	void OnGateChanged(UStateNode* Node, UMultiGateNodeGate* Gate);
};