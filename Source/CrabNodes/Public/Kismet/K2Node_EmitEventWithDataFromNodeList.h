#pragma once

#include "EdGraph/EdGraphNode.h"
#include "Internationalization/Text.h"
#include "Kismet/K2Node_EmitEventFromNodeList.h"

#include "K2Node_EmitEventWithDataFromNodeList.generated.h"

class FBlueprintActionDatabaseRegistrar;
class FString;
class UStateMachineInterface;
class UEdGraph;
class UEdGraphPin;
class UObject;
class UScriptStruct;
struct FLinearColor;

UCLASS()
class CRABNODES_API UK2Node_EmitEventWithDataFromNodeList : public UK2Node_EmitEventFromNodeList
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
