#pragma once

#include "LevelSequenceDirector.h"
#include "StateMachine/DataStructures.h"
#include "StateMachineSequenceDirector.generated.h"

class UDialogueStateComponent;
class UMonologueData;

UCLASS(Blueprintable)
class UStateMachineSequenceDirector : public ULevelSequenceDirector
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UDialogueStateComponent> Dialogue;

	UPROPERTY()
	TObjectPtr<UMonologueData> CurrentMonologue;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Events")
	FEventSlot ProgressDialogueEvent;

public:

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void PauseSequence();

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void ResumeSequence();

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void ProgressDialogue();

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void ProgressDialogueWithData(UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void ProgressDialogueWithPause();

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void ProgressDialogueWithPauseWithdata(UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void MachineEvent(FEventSlot InEvent);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StepMonologue();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void HideMonologue();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void LockMonologueToSection();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StepUnlockMonologue();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void UnlockMonologue();

protected:

	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void BindDialogueComponent();

private:

	UFUNCTION()
	void OnMonologueSpawned(UMonologueData* Monologue);

	UFUNCTION()
	void OnMonologueFinished(UMonologueData* Monologue);

};