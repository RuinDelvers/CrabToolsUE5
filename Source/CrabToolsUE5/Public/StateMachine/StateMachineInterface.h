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

	/*
	 * Node classes specified here are use both events they are expecting (notifies) and events that they emit.
	 * This has two applications: For notifies, this allows nodes to quickly be added to a SM, put that node class
	 * in its interface, then it's easily callable from the outside without defining new events.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess))
	TSet<TSoftClassPtr<UStateNode>> NodeEvents;

	/*
	 * For emitted events, this allows events from Hieararchy nodes that are passed SM's that dynamically instantiated
	 * to be used. For example, RPGTurnEnd nodes can be embedded in a dynamic SM, and this event would want to be useable
	 * in an extensible SM.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess))
	TSet<TSoftClassPtr<UStateNode>> NodeListeners;

	/* Do not read from this directly. Use GetEvents_Inner(). */
	mutable TSet<FName> NamespacedEvents;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Documentation",
		meta=(MultiLine=true))
	FString Description;

public:

	UStateMachineInterface();
	
	bool HasEvent(FName InEvent) const;
	bool HasCallEvent(FName InEvent) const;
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

		virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;

private:

	/* Verifies that reparenting with NewParent won't create a cycle. */
	bool VerifyNoCycles() const;

	void UpdateNamespacedEvents() const;
	const TSet<FName>& GetEvents_Inner() const;
};
