#pragma once

#include "SGraphPinNameList.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class UStateMachineInterface;
class UEdGraphPin;
class UStateMachine;
class UStateNode;

class CRABNODES_API SGraphPinSMEventName : public SGraphPinNameList
{
public:
	SLATE_BEGIN_ARGS(SGraphPinSMEventName) {}
	SLATE_END_ARGS()

	void Construct(
		const FArguments& InArgs,
		UEdGraphPin* InGraphPinObj,
		UStateMachineInterface* InInterface=nullptr,
		TSubclassOf<UStateMachine> SMClass=nullptr,
		TSubclassOf<UStateNode> SMNode=nullptr);

	void Construct(
		const FArguments& InArgs,
		UEdGraphPin* InGraphPinObj,
		TSubclassOf<UStateNode> SMNode);

	SGraphPinSMEventName();
	virtual ~SGraphPinSMEventName();

	void RefreshNameList(UStateMachineInterface* NewInterface);
	void RefreshNameList(TSubclassOf<UStateMachine> NewInterface);
	void RefreshNameList(UStateNode* Node);

};