#pragma once

#include "StateMachine/StateMachine.h"
#include "EventMapNode.generated.h"

/**
 * Node that maps incoming events to functions to be called on the State Machine.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UEventMapNode : public UStateNode
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE(FEventHandler);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FEventWithDataHandler, UObject*, Data);

private:

	UPROPERTY(EditAnywhere, Category = "StateMachine", 
		meta = (AllowPrivateAccess, GetKeyOptions="GetEventOptions", GetValueOptions="HandlerNameOptions"))
	TMap<FName, FName> EventMap;
	TMap<FName, FEventHandler> EventMapInternal;

	UPROPERTY(EditAnywhere, Category = "StateMachine", 
		meta = (AllowPrivateAccess, GetKeyOptions="GetEventOptions", GetValueOptions="HandlerWithDataNameOptions"))
	TMap<FName, FName> EventWithDataMap;
	TMap<FName, FEventWithDataHandler> EventWithDataMapInternal;

public:

	virtual void Event_Inner_Implementation(FName Event) override;
	virtual void EventWithData_Inner_Implementation(FName Event, UObject* Data) override;
	virtual void Initialize_Inner_Implementation() override;

private:

	UFUNCTION()
	TArray<FString> HandlerNameOptions() const;

	UFUNCTION()
	TArray<FString> HandlerWithDataNameOptions() const;

	UFUNCTION()
	void HandlerPrototype() {}

	UFUNCTION()
	void HandlerWithDataPrototype(UObject* Data) {}
};
