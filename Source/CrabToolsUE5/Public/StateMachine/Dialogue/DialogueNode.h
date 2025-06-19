// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Components/ActorComponent.h"
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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueSelected, const UDialogueData*, Data);
	FDialogueSelected OnDialogueSelected;

public:

	FORCEINLINE FName GetEvent() const { return this->Event; }
	FORCEINLINE const FText& GetDialogueText() const { return this->DialogueText; }

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void SelectedDialogue() const { this->OnDialogueSelected.Broadcast(this); }
};

USTRUCT(BlueprintType)
struct FDialogueDataStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category="Dialogue")
	TArray<TObjectPtr<UDialogueData>> DialogueData;
};

/* 
UINTERFACE(BlueprintType)
class UDialogueStateMachine : public UInterface
{
	GENERATED_BODY()
};

class IDialogueStateMachine
{
	GENERATED_BODY()

public:

	IDialogueStateMachine() = default;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="StateMachine|Dialogue")
	void BroadcastDialogueChoices(FDialogueDataStruct Choices);
	virtual void BroadcastDialogueChoices_Implementation(FDialogueDataStruct Choices)
	{
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "StateMachine|Dialogue")
	void ListenForDialogueChoices(const FListenForChoices_Single& Callback);
	virtual void ListenForDialogueChoices_Implementation(const FListenForChoices_Single& Callback)
	{
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dialogue")
	bool IsConversing() const;
	virtual bool IsConversing_Implementation() const { return false; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dialogue")
	bool HandShake(UObject* Conversee);
	virtual bool HandShake_Implementation(UObject* Conversee) { return false; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dialogue")
	UObject* GetInitializer() const;
	virtual UObject* GetInitializer_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dialogue")
	void GetAllDialogueActors(const TArray<UObject*> Conversers) const;
	virtual void GetAllDialogueActors_Implementation(const TArray<UObject*> Conversers) const {}
};
*/

//DECLARE_DYNAMIC_DELEGATE_OneParam(FListenForChoices_Single, FDialogueDataStruct, Choices);

/* This component can be used in conjuction of the DialogueNode & related to manage a dialogue state. */
UCLASS(Blueprintable, Category = "StateMachine|Dialogue",
	meta=(BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UDialogueStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FListenForChoices, FDialogueDataStruct, Choices);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FListenForDialogue);

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	TSet<TObjectPtr<UDialogueStateComponent>> Participants;

	UPROPERTY(BlueprintAssignable, Category="Dialogue")
	FListenForChoices OnChoicesSpawned;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FListenForDialogue OnDialogueStarted;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FListenForDialogue OnDialogueFinished;

public:

	bool HandShake(UDialogueStateComponent* Conversee);

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void AttemptDialogueWithActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void FinishDialogue();

private:

	void FinishDialogueInner();
};

UCLASS(Blueprintable, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UFinishDialogueNode : public UStateNode
{
	GENERATED_BODY()

private:

	/* 
	 * Whether or not when entering this node will it broadcast the end dialogue calls.	If it is false (default),
	 * then it will listen to its owner's dialogue component for the Finished callback before Emitting the DIALOGUE_FINISHED
	 * event. If it is broadcasting, it will still emit the event.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialogue", meta=(AllowPrivateAccess))
	bool bBroadcaster = false;

	UPROPERTY()
	TObjectPtr<UDialogueStateComponent> DialogueComponent;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;

	#if WITH_EDITOR
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif

private:

	UFUNCTION()
	void HandleDialogueFinished();
};

UCLASS(Blueprintable, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UConfirmDialogueNode : public UStateNode
{
	GENERATED_BODY()

protected:

	virtual void Initialize_Inner_Implementation() override;

	#if WITH_EDITOR
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif

private:

	UFUNCTION()
	void HandleDialogueStarted();
};

UCLASS(Blueprintable, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UAttemptDialogueNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<UDialogueStateComponent> DialogueComponent;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void EventWithData_Inner_Implementation(FName EName, UObject* Data) override;

	#if WITH_EDITOR
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif

private:

	void HandleComponent(UDialogueStateComponent* Comp);
};

/**
 * State Node that holds and broadcasts dialogue data through a statemachine that implements the
 * DialogueStateMachine interface. The interface can be passed either through EventWithData
 * or you can implement the interface for one of the owning state machines for this node, and
 * it'll send it through there.
 */
UCLASS(Blueprintable, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UDialogueNode : public UStateNode
{
	GENERATED_BODY()
	
	/* The map of choices text to the next state. */
	UPROPERTY(EditAnywhere, Category = "DialogueNode", meta = (AllowPrivateAccess = "true", ShowOnlyInnerProperties))
	FDialogueDataStruct Choices;

	UPROPERTY()
	TObjectPtr<UDialogueStateComponent> DialogueComponent;

public:

	//UDialogueNode();

protected:

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif
	
	virtual void Initialize_Inner_Implementation() override;
	//virtual void Tick_Implementation(float DeltaTime) override;
	//virtual void Event_Implementation(FName Event) override;
	//virtual void EventWithData_Inner_Implementation(FName EName, UObject* Data) override;
	//virtual void Enter_Implementation() override;
	//virtual void EnterWithData_Implementation(UObject* Data) override;
	//virtual void Exit_Implementation() override;	
	//virtual void ExitWithData_Implementation(UObject* Data) override;
	virtual void PostTransition_Inner_Implementation() override;

private:

	UFUNCTION()
	void HandleDialogueSelection(const UDialogueData* Data);
};