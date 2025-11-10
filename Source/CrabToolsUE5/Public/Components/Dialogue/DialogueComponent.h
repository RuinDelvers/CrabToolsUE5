#pragma once

#include "Components/ActorComponent.h"
#include "Sequencer/SequencerStruct.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueNode")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueNode",
		meta=(InlineEditConditionToggle))
	bool bApplySequenceAction = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueNode",
		meta=(EditCondition="bApplySequenceAction"))
	FSequencerAction Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "DialogueNode")
	TArray<TObjectPtr<UObject>> CustomData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DialogueNode")
	bool bUseLock = false;
};

UCLASS(Blueprintable, EditInlineNew, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UMonologueData : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monologue", meta = (AllowPrivateAccess = "true"))
	TArray<FMonologueDataStruct> MonologueText;

	int Index = -1;

	UPROPERTY()
	TObjectPtr<ULevelSequencePlayer> Player;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMonologueUpdated, UMonologueData*, Data);

	UPROPERTY(BlueprintAssignable, Category="Dialogue")
	FMonologueUpdated OnMonologueFinished;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FMonologueUpdated OnMonologueUpdate;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FMonologueUpdated OnMonologueHidden;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FMonologueUpdated OnMonologueLocksUpdated;

	/*
	 * This holds all the objects the monologue data is waiting on before 
	 * broadcasting an update.
	 */
	UPROPERTY()
	TSet<TObjectPtr<UObject>> Locks;

public:

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void Step(UObject* RemovedLock=nullptr);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AddLock(UObject* AddedLock);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void RemoveLock(UObject* AddedLock);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool IsLocked() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	const FMonologueDataStruct& Current() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FText CurrentText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Dialogue",
		meta=(ExpandBoolAsExecs="ReturnValue"))
	bool IsEmpty() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void Broadcast();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool Finished() const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ClearListeners(UObject* Obj);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetPlayer(ULevelSequencePlayer* InPlayer);

	UFUNCTION(BlueprintCallable, Category = "Dialogue", meta=(DeterminesOutputType="DataClass"))
	UObject* GetDataByClass(UClass* DataClass) const;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void GetAllDataByClass(UClass* DataClass, TArray<UObject*>& OutData) const;

	template <class T>
	UObject* GetDataByClass() const { return this->GetDataByClass(T::StaticClass()); }

	template <class T>
	void GetAllDataByClass(TArray<UObject*>& OutData) const { this->GetAllDataByClass(T::StaticClass(), OutData); }

private:
	
	void StepHelper(bool bIncrement);
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

	/*
	 * This event is used for communicating that there's no active
	 * dialogue currently. Useful for hiding UI elements which should
	 * not be visible during dialogue style cutscenes. This is
	 * generally handled outside the component, but is here for
	 * a simple centralized communication method.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FListenForDialogue OnDialogueNull;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FListenForDialogue OnDialogueFinished;

	UPROPERTY()
	TObjectPtr<UMonologueData> CurrentMonologue;

	UPROPERTY()
	TObjectPtr<UDialogueDataStruct> CurrentDialogue;

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

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SendDialogue(UDialogueDataStruct* DialogueData);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void NullDialogue() const;

	virtual void OnComponentDestroyed(bool bDestroyHierarchy) override;

private:

	void FinishDialogueInner();
	void ClearCachedData();
};