#pragma once

#include "Containers/Array.h"
#include "EdGraph/EdGraphNode.h"
#include "Internationalization/Text.h"
#include "Textures/SlateIcon.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/K2Node_EmitEventBase.h"
#include "StateMachine/DataStructures.h"
#include "K2Node_EmitEventFromInterface.generated.h"

class FBlueprintActionDatabaseRegistrar;
class FString;
class UStateMachineInterface;
class UEdGraph;
class UEdGraphPin;
class UObject;
class UScriptStruct;
struct FLinearColor;

UCLASS()
class CRABNODES_API UK2Node_EmitEventFromInterface : public UK2Node_EmitEventBase
{
	GENERATED_UCLASS_BODY()

protected:

	UPROPERTY(EditAnywhere, Category=Events)
	FEventSlot Event;

public:

	//~ Begin UEdGraphNode Interface.
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual FText GetTooltipText() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void PostReconstructNode() override;
	//~ End UEdGraphNode Interface.

	//~ Begin UK2Node Interface
	virtual bool IsNodeSafeToIgnore() const override { return true; }
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	//~ End UK2Node Interface

	UEdGraphPin* GetStateMachinePin() const;
	/** Get the then output pin */
	UEdGraphPin* GetThenPin() const;
	/** The hidden pin for the event. */	
	virtual UEdGraphPin* GetEventPin() const override;

	UFUNCTION()
	void UpdateSlotExtern(FEventSlot Slot);

	FEventSlot GetSlot() const { return this->Event; }

protected:

	void RefreshDefaultValue();

	/**
	 * Takes the specified "MutatablePin" and sets its 'PinToolTip' field (according
	 * to the specified description)
	 * 
	 * @param   MutatablePin	The pin you want to set tool-tip text on
	 * @param   PinDescription	A string describing the pin's purpose
	 */
	void SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const;

	/** Triggers a refresh which will update the node's widget; aimed at updating the dropdown menu for the Event input */
	void RefreshEventOptions();

	virtual bool ShouldShowNodeProperties() const { return true; }

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyEvent) override;
	#endif
};
