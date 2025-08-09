#pragma once

#include "StateMachine/StateMachine.h"
#include "CallFunctionNode.generated.h"

class UFunctionSource;
class UCallFunctionNode;

/**
 * Calls a function on either the state machine or the owner of this node. This is
 * intended to be a safer method of calling functions without using GenericPropertyBindings
 * as those are more tightly bound. This will allow for a variety of types to be
 * targeted, and data only blueprints of this node can be made and reused.
 */
UCLASS(Blueprintable, Category = "StateMachine|Functions")
class CRABTOOLSUE5_API UCallFunctionNode : public UStateNode
{
	GENERATED_BODY()

private:

	FMulticastScriptDelegate Callback;

	UPROPERTY(EditAnywhere, Category = "Functions")
	bool bCallOnEnter = true;

	UPROPERTY(EditAnywhere, Category = "Functions")
	bool bCallOnExit = false;

	UPROPERTY(EditAnywhere, Instanced, Category="Functions")
	TArray<TObjectPtr<UFunctionSource>> Functions;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

private:
	
	void HandleBinding(UObject* Obj, const TSet<FName>& FnNames);
};


UCLASS(Abstract, EditInlineNew, Within = "CallFunctionNode")
class UFunctionSource : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Filters", meta=(GetOptions="GetFunctionNames"))
	TSet<FName> Functions;

public:

	TArray<FScriptDelegate> GetBinding() const;

protected:

	virtual bool MatchesObject(UObject* Obj) const { return false; }
	virtual UClass* GetFilterClass() const { return nullptr; }
	virtual UObject* GetSource(const UCallFunctionNode* Node) const { return nullptr; }

	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetFunctionNames() const;

		UFUNCTION()
		void SignatureFunction() {}

		TArray<FString> GetClassFunctions(UClass* Classes) const;
	#endif
};

UCLASS(Within = "CallFunctionNode")
class UOwnerFunctionSource : public UFunctionSource
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Filters")
	TSubclassOf<UObject> Class = UObject::StaticClass();

protected:

	virtual bool MatchesObject(UObject* Obj) const override { return Obj->IsA(this->Class); }
	virtual UClass* GetFilterClass() const override { return this->Class; }
	virtual UObject* GetSource(const UCallFunctionNode* Node) const { return Node->GetOwner(); }
};

UCLASS(Within = "CallFunctionNode")
class UMachineFunctionSource : public UFunctionSource
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Filters")
	TSubclassOf<UStateMachine> Class = UStateMachine::StaticClass();

protected:

	virtual bool MatchesObject(UObject* Obj) const override { return Obj->IsA(this->Class); }
	virtual UClass* GetFilterClass() const override { return this->Class; }
	virtual UObject* GetSource(const UCallFunctionNode* Node) const { return Node->GetMachine(); }
};

UCLASS(Within = "CallFunctionNode")
class UParentMachineFunctionSource : public UMachineFunctionSource
{
	GENERATED_BODY()

protected:

	virtual UObject* GetSource(const UCallFunctionNode* Node) const { return Node->GetMachine()->GetParentMachine(); }
};

UCLASS(Within = "CallFunctionNode")
class URootMachineFunctionSource : public UMachineFunctionSource
{
	GENERATED_BODY()

protected:

	virtual UObject* GetSource(const UCallFunctionNode* Node) const { return Node->GetRootMachine(); }
};