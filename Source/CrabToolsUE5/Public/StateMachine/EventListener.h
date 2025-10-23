#pragma once

#include "CoreMinimal.h"
#include "EventListener.generated.h"

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UEventListenerInterface : public UInterface
{
	GENERATED_BODY()
};

class IEventListenerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="StateMachine")
	void Event(FName InEvent);
	virtual void Event_Implementation(FName InEvent) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="StateMachine")
	void EventWithData(FName InEvent, UObject* Data);
	virtual void EventWithData_Implementation(FName InEvent, UObject* Data) {}
};