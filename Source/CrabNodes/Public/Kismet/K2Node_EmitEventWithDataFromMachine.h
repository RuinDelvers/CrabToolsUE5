#pragma once

#include "Containers/Array.h"
#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "HAL/PlatformCrt.h"
#include "Internationalization/Text.h"
#include "K2Node.h"
#include "Textures/SlateIcon.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/K2Node_EmitEventFromInterface.h"

#include "K2Node_EmitEventWithDataFromMachine.generated.h"

class FBlueprintActionDatabaseRegistrar;
class FString;
class UStateMachineInterface;
class UEdGraph;
class UEdGraphPin;
class UObject;
class UScriptStruct;
struct FLinearColor;

UCLASS()
class CRABNODES_API UK2Node_EmitEventWithDataFromMachine : public UK2Node_EmitEventFromMachine
{
	GENERATED_UCLASS_BODY()

public:

	//~ Begin UEdGraphNode Interface.
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void PostReconstructNode() override;
	//~ End UEdGraphNode Interface.

	/** Get the Data Pin. */
	UEdGraphPin* GetDataPin() const;

};
