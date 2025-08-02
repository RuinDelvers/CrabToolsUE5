#pragma once

#include "StateMachine.h"
#include "CompositeNode.generated.h"

/* This is used to handle piped data for the nested structure. */
UCLASS()
class CRABTOOLSUE5_API UCompositeNodeData : public UObject, public IStateMachineDataInterface
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FName, TObjectPtr<UObject>> Data;

public:

	virtual UObject* FindDataOfType_Implementation(TSubclassOf<UObject> Type) override;
	virtual void FindAllDataOfType_Implementation(TSubclassOf<UObject> Type, TArray<UObject*>& ReturnValue) override;
	virtual TScriptInterface<UInterface> FindDataImplementing_Implementation(TSubclassOf<UInterface> Type) override;
	virtual void FindAllDataImplementing_Implementation(TSubclassOf<UInterface> Type, TArray<TScriptInterface<UInterface>>& ReturnValue) override;

	void AddData(FName Key, UObject* AddedData);
};

/**
 * State Machine Node that is a composite of other nodes.
 */
UCLASS(Blueprintable, CollapseCategories, HideDropdown, Category = "StateMachine")
class CRABTOOLSUE5_API UCompositeNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Instanced, Category = "CompositeNode", meta = (AllowPrivateAccess = "true"))
	TMap<FName, UStateNode*> Nodes;

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

	#if WITH_EDITORONLY_DATA
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif

};
