#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/DataStructures.h"
#include "StateMachineBlueprint.generated.h"

class UDataTable;
class UStateMachineInterface;

UCLASS(BlueprintType)
class STATEMACHINEEDITOR_API UStateMachineBlueprint
: public UBlueprint, 
	public IStateMachineLike
{
	GENERATED_UCLASS_BODY()

private:

	UPROPERTY()
	TObjectPtr<class UEdStateGraph> MainGraph;
	
	UPROPERTY()
	TArray<TObjectPtr<class UEdStateGraph>> SubGraphs;

	UPROPERTY(EditDefaultsOnly, Category="Interface")
	TSet<TSoftObjectPtr<UStateMachineInterface>> Interfaces;

	/* Which class to use for state data. */
	UPROPERTY(EditDefaultsOnly, Category = "StateMachine")
	TSubclassOf<UState> DefaultStateClass;

public:

	/* Editor Events when this object is changed. */
	class FStateMachineBlueprintEditorEvents
	{
	public:

		DECLARE_MULTICAST_DELEGATE_OneParam(FGraphSelected, UEdStateGraph*)
		FGraphSelected OnGraphSelected;

		DECLARE_MULTICAST_DELEGATE_OneParam(FObjectInspected, UObject* Obj)
		FObjectInspected OnObjectInspected;
	}
	Events;

public:

	/* Stores event names mapped to their descriptions from this SMBP. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "StateMachine")
	void GetEventEntries(TMap<FName, FEventSetRow>& Entries);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "StateMachine")
	void AddEventsToDataTable(UDataTable* EventSet, bool bClearEvents=false);

	void SelectGraph(UEdStateGraph* Graph);
	UClass* GetBlueprintClass() const override;
	virtual bool SupportsInputEvents() const override;

	UEdStateGraph* GetMainGraph();
	UEdStateGraph* AddSubGraph();

	bool IsGraphNameAvailable(FString& Name) const;
	bool IsEventNameAvailable(FName Name) const;
	bool HasEvent(FName EName) const;
	bool IsMainGraph(const UEdStateGraph* Graph) const;
	void RenameGraph(UEdStateGraph* Graph, FName Name);
	void DeleteGraph(UEdStateGraph* Graph);
	void ClearDelegates();
	TSubclassOf<UState> GetStateClass() const { return this->DefaultStateClass; }
	const TArray<class UEdStateGraph*>& GetSubgraphs() { return this->SubGraphs; }
	/* Returns the set of events defined by this SMBP. */
	TSet<FName> GetEventSet() const;

	// IStateMachineLike Interface
	virtual TArray<FString> GetMachineOptions() const override;
	virtual TArray<FString> GetPropertiesOptions(const FSMPropertySearch& SearchParam) const override;

	UStateMachineBlueprintGeneratedClass* GetStateMachineGeneratedClass() const;

	/* Returns the submachines that explicitly defined in this blueprint. */
	TArray<FString> GetDefinedSubMachines() const;

	FName GetNewGraphName() const;

	TSet<TSoftObjectPtr<UStateMachineInterface>> GetInterfaces() const { return this->Interfaces; }

	void Verify(FNodeVerificationContext& Context) const;
	void AppendInterfaceEvents(TArray<FString>& Names) const;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(
			FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif	

	virtual void SetObjectBeingDebugged(UObject* Obj) override;

private:

	void InspectObject(UObject* Obj);

	void UpdateDefaultStateClass();
};