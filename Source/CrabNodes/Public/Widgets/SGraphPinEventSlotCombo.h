#pragma once

#include "SGraphPin.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class UStateMachineInterface;
class UEdGraphPin;
class UStateMachine;
class UStateNode;
class UK2Node_EmitEventFromInterface;

class CRABNODES_API SGraphPinEventSlotCombo : public SGraphPin
{

private:

	TWeakObjectPtr<UK2Node_EmitEventFromInterface> Node;

public:
	SLATE_BEGIN_ARGS(SGraphPinSMEventName) {}
	SLATE_END_ARGS()

	void Construct(
		const FArguments& InArgs,
		UK2Node_EmitEventFromInterface* InNode);

	SGraphPinEventSlotCombo();
	virtual ~SGraphPinEventSlotCombo() {}
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

private:

};