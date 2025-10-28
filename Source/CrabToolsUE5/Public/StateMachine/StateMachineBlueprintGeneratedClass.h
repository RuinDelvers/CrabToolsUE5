#pragma once

#include "UObject/ObjectMacros.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/StateMachineEnum.h"
#include "StateMachineBlueprintGeneratedClass.generated.h"

class UStateMachineInterface;

USTRUCT()
struct CRABTOOLSUE5_API FStateArchetypeData
{
	GENERATED_BODY()

	friend class UStateMachineBlueprintGeneratedClass;

private:

	UPROPERTY()
	TObjectPtr<UState> Archetype;

public:

	FStateArchetypeData() {}
	FStateArchetypeData(UState* Machine) : Archetype(Machine) {}

	UPROPERTY()
	bool bIsOverride = false;

	UPROPERTY()
	bool bIsExtension = false;

	UPROPERTY()
	EStateMachineAccessibility Access = EStateMachineAccessibility::PRIVATE;

	const UState* GetArchetype() const { return this->Archetype; }
	UState* GetArchetype() { return this->Archetype; }
	void SetArchetype(UState* NewState) { this->Archetype = NewState; }
};

USTRUCT()
struct CRABTOOLSUE5_API FStateMachineArchetypeData
{
	GENERATED_BODY()

	friend class UStateMachineBlueprintGeneratedClass;

private:

	UPROPERTY()
	TObjectPtr<UStateMachine> Archetype;

	#if WITH_EDITORONLY_DATA
		UPROPERTY()
		TSet<FName> EventSet;
	#endif

public:

	FStateMachineArchetypeData() {}
	FStateMachineArchetypeData(UStateMachine* Machine) : Archetype(Machine) {}

	UPROPERTY()
	FName StartState;

	UPROPERTY()
	TMap<FName, FStateArchetypeData> StateData;

	UPROPERTY()
	bool bInstanceEditable = false;

	UPROPERTY()
	bool bIsVariable = false;

	UPROPERTY()
	bool bCanOverrideStart = false;

	UPROPERTY()
	EStateMachineAccessibility Accessibility = EStateMachineAccessibility::PRIVATE;

	void AddStateData(FName StateName, FStateArchetypeData Data);
	const UStateMachine* GetArchetype() const { return this->Archetype; }
	UStateMachine* GetArchetype() { return this->Archetype; }
	void SetArchetype(UStateMachine* NewMachine) { this->Archetype = NewMachine; }
	UStateMachine* CreateStateMachine(UStateMachine* Parent, FName ParentKey) const;
	void CleanAndSanitize();

	#if WITH_EDITORONLY_DATA
		void AppendEvents(const TSet<FName> NewEvents) { this->EventSet.Append(NewEvents); }
	#endif
};

UCLASS()
class CRABTOOLSUE5_API UStateMachineBlueprintGeneratedClass : public UBlueprintGeneratedClass
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FStateMachineArchetypeData Archetype;

	UPROPERTY()
	TMap<FName, FStateMachineArchetypeData> SubArchetypes;

	#if WITH_EDITORONLY_DATA
		UPROPERTY()
		TSet<TObjectPtr<UStateMachineInterface>> Interfaces;
	#endif //WITH_EDITORONLY_DATA

public:


	void AppendEventEmitters(UStateMachine* SM);
	
	UState* GetStateData(UStateMachine* Outer, FName Machine, FName StateName);
	void AppendPublicStateNames(TSet<FName>& Names) const;
	TArray<FString> GetStateOptions(EStateMachineAccessibility Access = EStateMachineAccessibility::PUBLIC) const;
	TArray<FString> GetChildAccessibleSubMachines() const;
	EStateMachineAccessibility GetSubMachineAccessibility(FName Key) const;
	TArray<FName> GetSubMachineOptions() const;
	UStateMachine* ConstructSubMachine(UStateMachine* Outer, FName Key) const;
	FName GetStartState(FName MachineName = NAME_None) const;
	TSet<FName> GetEventSet(FName MachineName = NAME_None) const;
	TSet<FName> GetTotalEventSet() const;

	/*
	 * Returns whether or not the given event was defined in the given machine. Does not search
	 * interfaces.
	 */
	bool HasEvent(FName InEvent, FName MachineName = NAME_None) const;
	bool HasInterfaceEvent(FName InEvent) const;
	void AppendInterfaceEvents(TArray<FString>& Names) const;

	void CollectExtendibleStates(TSet<FString>& StateNames, FName SubMachineName = NAME_None) const;
	void CollectOverrideableStates(TSet<FString>& StateNames, FName SubMachineName = NAME_None) const;

	// Archetype Information functions
	const FStateMachineArchetypeData* GetMachineArchetypeData(FName MachineName = NAME_None) const;
	const FStateArchetypeData* GetStateArchetypeData(FName StateName, FName MachineName = NAME_None) const;
	// End Archetypal information functions

	UStateMachineBlueprintGeneratedClass* GetParent() const
	{
		return Cast<UStateMachineBlueprintGeneratedClass>(this->GetSuperClass());
	}

	bool IsSubMachineNameInUse(FString& Name) const;

	UStateMachine* GetMostRecentParentArchetype(FName SubMachineKey) const;

	// Procedural Construction Functions
	void CleanAndSanitize();
	void AddStateMachine(FStateMachineArchetypeData Data, FName MachineName = NAME_None);
	void VerifyClass(FNodeVerificationContext& Context);

	void GetStateEmittedEvents(FName Machine, FName State, TSet<FName>& OutNames) const;

	#if WITH_EDITOR
		bool DoesImplementInterface(UStateMachineInterface* Interface) const;
	#endif

private:
	FName GetStartState_Local(FName MachineName, bool& CanOverride) const;
	FName GetStartState_Inner(FName MachineName, bool& CanOverride) const;
};