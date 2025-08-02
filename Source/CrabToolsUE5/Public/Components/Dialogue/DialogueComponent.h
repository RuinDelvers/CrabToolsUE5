#pragma once

#include "Components/ActorComponent.h"

#include "DialogueComponent.generated.h"

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

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SelectedDialogue() const { this->OnDialogueSelected.Broadcast(this); }
};

/* Used to broadcast dialogue data through BP interfaces without reference warnings. */
UCLASS(BlueprintType)
class UDialogueDataStruct : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Dialogue")
	TArray<TObjectPtr<UDialogueData>> DialogueData;
};


USTRUCT(BlueprintType)
struct FMonologueDataStruct
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueNode", meta = (AllowPrivateAccess = "true"))
	FText Text;

	/* Used to determine if the monologue should auto step. If greater than 0, it'll make a timer. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueNode", meta = (AllowPrivateAccess = "true"))
	float Timeout = 0.0;
};

UCLASS(Blueprintable, EditInlineNew, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UMonologueData : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mono", meta = (AllowPrivateAccess = "true"))
	TArray<FMonologueDataStruct> MonologueText;

	/* Timeout used for all parts of the monologue sequence. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mono", meta = (AllowPrivateAccess = "true"))
	float Timeout = 0.0;

	UPROPERTY()
	int Index = -1;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMonologueUpdated, UMonologueData*, Data);

	UPROPERTY(BlueprintAssignable, Category="Dialogue")
	FMonologueUpdated OnMonologueFinished;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FMonologueUpdated OnMonologueUpdate;

public:

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool Step();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FText CurrentText() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool Finished() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ClearListeners(UObject* Obj);
};

/* This component can be used in conjuction of the DialogueNode & related to manage a dialogue state. */
UCLASS(Blueprintable, Category = "StateMachine|Dialogue",
	meta = (BlueprintSpawnableComponent))
	class CRABTOOLSUE5_API UDialogueStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FListenForChoices, UDialogueDataStruct*, Choices);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FListenForMonologue, UMonologueData*, Data);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FListenForDialogue);

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	TSet<TObjectPtr<UDialogueStateComponent>> Participants;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FListenForChoices OnChoicesSpawned;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FListenForMonologue OnMonologueSpawned;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FListenForDialogue OnDialogueStarted;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FListenForDialogue OnDialogueFinished;

public:

	bool HandShake(UDialogueStateComponent* Conversee);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AttemptDialogueWithActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void FinishDialogue();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SendMonologue(UMonologueData* Data);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool IsInDialogue() const;

private:

	void FinishDialogueInner();
};