#pragma once

#include "SGraphPin.h"
#include "EdGraph/EdGraphPin.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class UStateMachineInterface;
class UEdGraphPin;
class UStateMachine;
class UStateNode;

class CRABNODES_API SGraphPinEventSlotCombo : public SGraphPin
{

private:

public:
	SLATE_BEGIN_ARGS(SGraphPinSMEventName) {}
	SLATE_END_ARGS()

	void Construct(
		const FArguments& InArgs,
		UEdGraphPin* InPin);

	SGraphPinEventSlotCombo();
	virtual ~SGraphPinEventSlotCombo() {}
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

private:

};