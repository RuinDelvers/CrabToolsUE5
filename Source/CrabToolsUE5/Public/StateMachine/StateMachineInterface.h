#pragma once

#include "StateMachineInterface.generated.h"

class UStateNode;

USTRUCT(BlueprintType)
struct FSMIData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateMachine", meta = (MultiLine = true))
	FName Category;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess))
	TSet<TSoftClassPtr<UStateNode>> NodeEvents;

	/* Do not read from this directly. Use GetEvents_Inner(). */
	mutable TSet<FName> NamespacedEvents;

public:

	UStateMachineInterface();
	
	bool HasEvent(FName InEvent) const;
	bool HasCallEvent(FName InEvent) const;
	TSet<FName> GetCallEvents() const;
	TSet<FName> GetEvents() const;
	const TMap<FName, FSMIData> GetEventData() const { return this->Events; }

	void AddEvent(FName InEvent) { this->Events.Add(InEvent); }

	TSet<FName> GetNodeEvents() const;

	void SetParent(UStateMachineInterface* NewParent);
	void SetParent(TSoftObjectPtr<UStateMachineInterface> NewParent);

	FName EventToNamespaced(FName EventName) const;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PreEditChange(FProperty* Property) override;
	#endif //WITH_EDITOR

private:

	/* Verifies that reparenting with NewParent won't create a cycle. */
	bool VerifyNoCycles() const;

	void UpdateNamespacedEvents() const;
	const TSet<FName>& GetEvents_Inner() const;
};
