#pragma once

#include "SGraphPinNameList.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class UStateMachineInterface;
class UEdGraphPin;
class UStateMachine;
class UStateNode;
class UK2Node_EmitEventBase;

class CRABNODES_API SGraphPinSMEventName : public SGraphPinNameList
{
public:
	SLATE_BEGIN_ARGS(SGraphPinSMEventName) {}
	SLATE_END_ARGS()

	void Construct(
		const FArguments& InArgs,
		UEdGraphPin* InGraphPinObj,
		UK2Node_EmitEventBase* Node);

	SGraphPinSMEventName();
	virtual ~SGraphPinSMEventName();

private:

	void RefreshNameList(const TSet<FName>& Names);
};