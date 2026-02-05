#pragma once

#include "StateMachine/StateMachine.h"
#include "SequentialNode.generated.h"

/**
 * Node that sequentially moves through node behaviours each time it is visited.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API USequentialNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Nodes")
	bool bLoop = false;

	UPROPERTY(EditAnywhere, Instanced, Category="Nodes")
	TArray<TObjectPtr<UStateNode>> Nodes;

	TObjectPtr<UStateNode> CurrentNode;
	int Index = -1;

public:

protected:
	
	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Event_Inner_Implementation(FName InEvent) override;
	virtual void EventWithData_Inner_Implementation(FName InEvent, UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Initialize_Inner_Implementation() override;
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
