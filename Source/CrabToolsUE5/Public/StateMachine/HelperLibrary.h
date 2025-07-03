#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StateMachine/EventListener.h"
#include "HelperLibrary.generated.h"

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
	static void EmitEvent(UObject* Obj, FName EName);

	/* Primarily used for K2_Nodes that may need to bind to a function. */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="StateMachine")
	static void EmitEventWithData(UObject* Obj, FName EName, UObject* Data);
};
