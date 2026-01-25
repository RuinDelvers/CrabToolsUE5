#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StateMachine/EventListener.h"
#include "HelperLibrary.generated.h"

struct FEventSlot;

/**
 * 
 */
UCLASS()
class CRABTOOLSUE5_API UStateMachineHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/* Primarily used for K2_Nodes that may need to bind to a function. */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="StateMachine")
	static void EmitEvent(UObject* Obj, FName InEvent);

	/* Primarily used for K2_Nodes that may need to bind to a function. */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="StateMachine")
	static void EmitEventWithData(UObject* Obj, FName InEvent, UObject* Data);

	/* Sends the given even to the given object. Will do nothing if the object doesn't implement EventListener. */
	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta=(DisplayName="Send Event"))
	static void EmitEventSlot(UObject* EventListener, FEventSlot InEvent);

	/* Sends the given even to the given object. Will do nothing if the object doesn't implement EventListener. */
	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (DisplayName = "Send Event With Data"))
	static void EmitEventSlotWithData(UObject* EventListener, FEventSlot InEvent, UObject* Data);
};
