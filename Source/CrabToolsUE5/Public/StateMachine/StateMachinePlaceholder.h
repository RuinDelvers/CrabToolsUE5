#pragma once

#include "CoreMinimal.h"
#include "StateMachine.h"
#include "StateMachinePlaceholder.generated.h"

/**
 * State Machine Node used as a mapped placeholder to be replaced. Has no other functionality.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UStateMachinePlaceholderNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="StateMachine")
	FName SlotName;

	UPROPERTY()
	TObjectPtr<UStateMachine> SubMachine;

public:

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	FORCEINLINE FName GetSlotName() { return this->SlotName; }

	virtual void Initialize_Inner_Implementation() override;

	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Event_Inner_Implementation(FName Event) override;
	virtual void EventWithData_Inner_Implementation(FName InEvent, UObject* Data);
	virtual void Enter_Inner_Implementation();
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation();
	virtual void ExitWithData_Inner_Implementation(UObject* Data) override;
};
