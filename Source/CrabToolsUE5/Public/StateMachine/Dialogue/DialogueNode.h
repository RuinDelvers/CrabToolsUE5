// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StateMachine/Dialogue/AbstractDialogueNode.h"
#include "DialogueNode.generated.h"

UCLASS(Blueprintable, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UFinishDialogueNode : public UAbstractDialogueNode
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

/*
 * This node is designed to coordinate another acting initiating dialogie with the owner of
 * this node. While you can technically directly attempt a handshake directly with a component,
 * you may not always want that to be possible, and this node can be placed in subgraphs to handle
 * it when it is possible.
 */
UCLASS(Blueprintable, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UAttemptDialogueNode : public UAbstractDialogueNode
{
	GENERATED_BODY()

private:

protected:

	virtual void EventWithData_Inner_Implementation(FName InEvent, UObject* Data) override;

	#if WITH_EDITOR
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif

	UFUNCTION()
	void EventNotify_AttemptDialogue(FName InEvent, UObject* Data);

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
class CRABTOOLSUE5_API UDialogueNode : public UAbstractDialogueNode
{
	GENERATED_BODY()
	
	/* The map of choices text to the next state. */
	UPROPERTY(VisibleAnywhere, Category = "DialogueNode", meta = (AllowPrivateAccess = "true", ShowInnerProperties))
	TObjectPtr<UDialogueDataStruct> Choices;

	UPROPERTY(EditAnywhere, Category="DialogueNode", meta=(AllowPrivateAccess))
	bool bNullOnExit = false;

public:

	UDialogueNode();

protected:

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif
	
	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

private:

	UFUNCTION()
	void HandleDialogueSelection(const UDialogueData* Data);
};