#pragma once

#include "K2Node.h"
#include "UObject/ObjectMacros.h"
#include "GameplayTagContainer.h"
#include "K2Node_EmitEventBase.generated.h"

UCLASS(Abstract)
class CRABNODES_API UK2Node_EmitEventBase : public UK2Node
{
	GENERATED_BODY()

protected:

	/** Tooltip text for this node. */
	FText NodeTooltip;

public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FEventSetChanged, const TSet<FName>&)
	FEventSetChanged OnEventSetChanged;

public:

	UK2Node_EmitEventBase(const FObjectInitializer& ObjectInitializer);

	virtual TSet<FName> GetEventSet() const { return {}; }
	virtual bool IsActionFilteredOut(FBlueprintActionFilter const& Filter) override;

	virtual UEdGraphPin* GetEventPin() const { return nullptr; }

	bool CheckValidEvent(class FCompilerResultsLog& CompilerContext) const;

private:

	void PrintInvalidEventError(class FCompilerResultsLog& CompilerContext, FName EName) const;
};