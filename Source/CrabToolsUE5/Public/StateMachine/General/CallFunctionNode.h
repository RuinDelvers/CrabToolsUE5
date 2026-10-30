#pragma once

#include "StateMachine/StateMachine.h"
#include "Properties/GenericPropertyBinding.h"
#include "CallFunctionNode.generated.h"

class UCallFunctionNode;

USTRUCT()
struct FCallFunctionNodeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Functions")
	bool bCallOnEnter = true;

	UPROPERTY(EditAnywhere, Category = "Functions")
	bool bCallOnExit = false;

	UPROPERTY(EditAnywhere, Category = "Functions")
	FGenericFunctionPropertyBinding Function;
};

/**
 * Calls a function defined by property bindings.
 */
UCLASS(Blueprintable, Category = "StateMachine|Functions")
class CRABTOOLSUE5_API UCallFunctionNode : public UStateNode
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "Functions")
	TArray<FCallFunctionNodeData> Functions;

protected:
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

private:
	
	void HandleBinding(UObject* Obj, const TSet<FName>& FnNames);
};