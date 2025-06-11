// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "StateMachine/StateMachine.h"
#include "DialogueNode.generated.h"

/* A reference to a choice*/
UCLASS(Blueprintable, EditInlineNew, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UDialogueData : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueNode", meta = (AllowPrivateAccess = "true"))
	FText DialogueText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueNode", meta = (AllowPrivateAccess = "true"))
	FName Event;

public:

	FORCEINLINE FName GetEvent() const { return this->Event; }
	FORCEINLINE const FText& GetDialogueText() const { return this->DialogueText; }
};

USTRUCT(BlueprintType)
struct FDialogueDataStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category="Dialogue")
	TArray<TObjectPtr<UDialogueData>> DialogueData;
};

/* Implement this for statemachines that you wish to utilize as dialogue SMs.*/
UINTERFACE(BlueprintType)
class UDialogueStateMachine : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FListenForChoices_Multi, FDialogueDataStruct, Choices);
DECLARE_DYNAMIC_DELEGATE_OneParam(FListenForChoices_Single, FDialogueDataStruct, Choices);

class IDialogueStateMachine
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category="StateMachine|Dialogue")
	void BroadcastDialogueChoices(FDialogueDataStruct Choices);
	void BroadcastDialogueChoices_Implementation(FDialogueDataStruct Choices) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "StateMachine|Dialogue")
	void ListenForDialogueChoices(const FListenForChoices_Single& Callback);
	void ListenForDialogueChoices_Implementation(const FListenForChoices_Single& Callback) {}
};


/**
 * State Machine Node that is a composite of other nodes.
 */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UDialogueNode : public UStateNode
{
	GENERATED_BODY()
	
	/* The map of choices text to the next state. */
	UPROPERTY(EditAnywhere, Category = "DialogueNode", meta = (AllowPrivateAccess = "true"))
	FDialogueDataStruct Choices;

public:

protected:

	#if WITH_EDITOR
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif
	
	//virtual void Initialize_Implementation(UStateMachine* POwner) override;
	//virtual void Tick_Implementation(float DeltaTime) override;
	//virtual void Event_Implementation(FName Event) override;
	//virtual void EventWithData_Implementation(FName EName, UObject* Data) override;
	//virtual void Enter_Implementation() override;
	//virtual void EnterWithData_Implementation(UObject* Data) override;
	//virtual void Exit_Implementation() override;	
	//virtual void ExitWithData_Implementation(UObject* Data) override;
	virtual void PostTransition_Inner_Implementation() override;
};