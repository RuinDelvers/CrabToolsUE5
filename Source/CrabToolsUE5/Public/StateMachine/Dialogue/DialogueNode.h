// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StateMachine/StateMachine.h"
#include "Components/Dialogue/DialogueComponent.h"
#include "DialogueNode.generated.h"

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
	UPROPERTY(VisibleAnywhere, Category = "DialogueNode", meta = (AllowPrivateAccess = "true", ShowInnerProperties))
	TObjectPtr<UDialogueDataStruct> Choices;

	UPROPERTY()
	TObjectPtr<UDialogueStateComponent> DialogueComponent;

public:

	UDialogueNode();

protected:

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif
	
	virtual void Initialize_Inner_Implementation() override;
	virtual void PostTransition_Inner_Implementation() override;

private:

	UFUNCTION()
	void HandleDialogueSelection(const UDialogueData* Data);
};