#pragma once

#include "K2Node.h"
#include "K2Node_CustomInteraction.generated.h"

UCLASS()
class CRABNODES_API UK2Node_CustomInteraction : public UK2Node
{
	GENERATED_BODY()

public:

	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

private:

	UFUNCTION()
	static void SignatureFunction(AActor* Interactor, UObject* Data) {}
};