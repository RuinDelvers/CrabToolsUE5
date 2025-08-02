#pragma once

#include "StateMachine.h"
#include "ArrayNode.generated.h"

/* This is used to handle piped data for the nested structure. */
UCLASS()
class CRABTOOLSUE5_API UArrayNodeData : public UObject, public IStateMachineDataInterface
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<UObject>> Data;

public:

	virtual UObject* FindDataOfType_Implementation(TSubclassOf<UObject> Type) override;
	virtual void FindAllDataOfType_Implementation(TSubclassOf<UObject> Type, TArray<UObject*>& ReturnValue) override;
	virtual TScriptInterface<UInterface> FindDataImplementing_Implementation(TSubclassOf<UInterface> Type) override;
	virtual void FindAllDataImplementing_Implementation(TSubclassOf<UInterface> Type, TArray<TScriptInterface<UInterface>>& ReturnValue) override;

	void AddData(UObject* AddedData);
};

/**
 * State Machine Node that is a composite of other nodes.
 */
UCLASS(Blueprintable, CollapseCategories, HideDropdown, Category = "StateMachine")
class CRABTOOLSUE5_API UArrayNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, Category = "ArrayNode", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UStateNode>> Nodes;

public:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Event_Inner_Implementation(FName Event) override;
	virtual void EventWithData_Inner_Implementation(FName EName, UObject* Data) override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;	
	virtual void ExitWithData_Inner_Implementation(UObject* Data) override;
	virtual bool RequiresTick_Implementation() const override;
	virtual void PostTransition_Inner_Implementation() override;
	virtual bool HasPipedData_Implementation() const override;
	virtual UObject* GetPipedData_Implementation() override;
	virtual void SetActive_Inner_Implementation(bool bNewActive) override;

	FORCEINLINE void AddNode(UStateNode* Node) { this->Nodes.Add(Node); }

	#if WITH_EDITORONLY_DATA
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif
};
