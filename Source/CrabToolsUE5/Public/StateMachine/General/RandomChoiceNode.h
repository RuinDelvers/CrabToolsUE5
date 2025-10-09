#pragma once

#include "StateMachine/StateMachine.h"
#include "RandomChoiceNode.generated.h"

USTRUCT()
struct FRandomChoiceNodeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, Category="Node")
	TObjectPtr<UStateNode> Node;

	/* How many times this node will be used for entry. */
	UPROPERTY(EditAnywhere, Category = "Node")
	int LifeTime = 1;
};

/**
 * A node that random chooses its behaviour each time it is entered.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API URandomChoiceNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Nodes",
		meta=(ShowOnlyInnterProperties))
	TArray<FRandomChoiceNodeData> Nodes;

	TObjectPtr<UStateNode> CurrentNode;
	int Index = -1;
	int CurrentLife = -1;

public:

protected:
	
	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Event_Inner_Implementation(FName EName) override;
	virtual void EventWithData_Inner_Implementation(FName EName, UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Initialize_Inner_Implementation() override;
	virtual void PostTransition_Inner_Implementation() override;
	virtual bool RequiresTick_Implementation() const override;
	virtual bool HasPipedData_Implementation() const override;
	virtual void SetActive_Inner_Implementation(bool bNewActive) override;
	virtual UObject* GetPipedData_Implementation();

	#if WITH_EDITOR
		virtual void GetNotifies(TSet<FName>& Events) const override;
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;		
	#endif

private:

	void Increment();
};
