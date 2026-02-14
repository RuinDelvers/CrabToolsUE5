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
	void Event(FName InEvent, UObject* Source);
	virtual void Event_Implementation(FName InEvent, UObject* Source) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="StateMachine")
	void EventWithData(FName InEvent, UObject* Data, UObject* Source);
	virtual void EventWithData_Implementation(FName InEvent, UObject* Data, UObject* Source) {}
};