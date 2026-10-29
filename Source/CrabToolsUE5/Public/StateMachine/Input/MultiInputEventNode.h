#pragma once

#include "StateMachine/Input/BaseInputNode.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "InputTriggers.h"
#include "StateMachine/DataStructures.h"
#include "MultiInputEventNode.generated.h"

class UInputAction;

USTRUCT()
struct FInputEventMap
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TMap<ETriggerEvent, FEventDataStruct> Events;

	/* This is need for same frame state loops to prevent rebinding when unnecessary. */
	uint64 LastFrame = 0;
};

/**
 * This input node will map InputActions to events to be emitted. Each action is limited to fire once per frame.
 */
UCLASS(Blueprintable, EditInlineNew, DontCollapseCategories, Category = "StateMachine|Input")
class CRABTOOLSUE5_API UMultiInputEventNode : public UBaseInputNode
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TMap<TObjectPtr<UInputAction>, FInputEventMap> EventMap;
	TArray<uint32> Handles;

public:

	UFUNCTION(BlueprintCallable, Category="Input")
	void BindInput();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void UnbindInput();

protected:

	virtual void SetActive_Inner_Implementation(bool bNewActive) override;

	#if WITH_EDITOR
		virtual void GetEmittedEvents(TSet<FName>& Events) const;
	#endif

private:

	void EmitDefinedEvent(const FInputActionValue& Value, FEventDataStruct Data, UInputAction* Action);
};