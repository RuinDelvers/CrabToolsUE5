#pragma once

#include "CoreMinimal.h"
#include "EventEmitter.generated.h"

class UStateMachine;

/**
 * Class which is designed to be a pluggable event emitter for external events.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, Category = "StateMachine|Events")
class CRABTOOLSUE5_API UEventEmitter : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY(Transient)
	TObjectPtr<UStateMachine> Machine;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Events")
	TSet<FName> EmittedEvents;
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="StateMachine")
	UStateMachine* GetMachine() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	AActor* GetOwner() const;

	void Initialize(UStateMachine* POwner);

	const TSet<FName>& GetEmittedEvents() const { return this->EmittedEvents; }

protected:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="StateMachine|Events")
	void Initialize_Inner();
	void Initialize_Inner_Implementation() {}

	UFUNCTION(BlueprintCallable, Category="Events")
	void EmitEvent(FName Event);
};
