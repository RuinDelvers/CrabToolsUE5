#pragma once

#include "StateMachineInterface.generated.h"

class UStateNode;

USTRUCT(BlueprintType)
struct FSMIData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateMachine", meta=(MultiLine=true))
	FText Description;
};

/**
 * Simple storage class that contains what public interface a statemachine has.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UStateMachineInterface : public UObject
{
	GENERATED_BODY()

private:

	/* Used for Pre and Post edit changes. */
	TSoftObjectPtr<UStateMachineInterface> ParentBackup;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess))
	TSoftObjectPtr<UStateMachineInterface> Parent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="StateMachine",
		meta=(AllowPrivateAccess))
	TMap<FName, FSMIData> Events;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="StateMachine",
		meta=(AllowPrivateAccess))
	TMap<FName, FSMIData> States;

	/* Set of public submachines implemented */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="StateMachine",
		meta=(AllowPrivateAccess))
	TMap<FName, FSMIData> SubMachines;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess))
	TSet<TSoftClassPtr<UStateNode>> NodeEvents;

public:

	
	bool HasEvent(FName EName) const;
	TSet<FName> GetCallEvents() const;
	TSet<FName> GetEvents() const;
	TSet<FName> GetStates() const;
	TSet<FName> GetSubMachines() const;

	void AddEvent(FName EName) { this->Events.Add(EName); }
	void AddState(FName SName) { this->States.Add(SName); }

	void AddSubMachine(FName SName) { this->SubMachines.Add(SName);	}

	void SetParent(UStateMachineInterface* NewParent);
	void SetParent(TSoftObjectPtr<UStateMachineInterface> NewParent);

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PreEditChange(FProperty* Property) override;
	#endif //WITH_EDITOR

private:

	/* Verifies that reparenting with NewParent won't create a cycle. */
	bool VerifyNoCycles() const;
};
