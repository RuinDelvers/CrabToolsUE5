#include "Widgets/SGraphPinSMEventName.h"

#include "Containers/Array.h"
#include "StateMachine/StateMachineInterface.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/StateMachine.h"
#include "Templates/SharedPointer.h"
#include "UObject/NameTypes.h"

class UEdGraphPin;

void SGraphPinSMEventName::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj, UStateMachineInterface* InInterface, TSubclassOf<UStateMachine> SMClass, TSubclassOf<UStateNode> Node)
{
	if (IsValid(InInterface))
	{
		RefreshNameList(InInterface);
	}
	else if (SMClass)
	{

	}
	else if (Node)
	{
		RefreshNameList(Cast<UStateNode>(Node->GetDefaultObject()));
	}

	SGraphPinNameList::Construct(SGraphPinNameList::FArguments(), InGraphPinObj, NameList);
}

void SGraphPinSMEventName::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj, TSubclassOf<UStateNode> Node)
{
	if (Node)
	{
		RefreshNameList(Cast<UStateNode>(Node->GetDefaultObject()));
	}

	SGraphPinNameList::Construct(SGraphPinNameList::FArguments(), InGraphPinObj, NameList);
}

SGraphPinSMEventName::SGraphPinSMEventName()
{
}

SGraphPinSMEventName::~SGraphPinSMEventName()
{
}


void SGraphPinSMEventName::RefreshNameList(UStateMachineInterface* Interface)
{
	NameList.Empty();

	if (Interface)
	{		
		auto Names = Interface->GetEvents();
		for (auto& Name : Names)
		{
			TSharedPtr<FName> RowNameItem = MakeShareable(new FName(Name));
			NameList.Add(RowNameItem);
		}
	}
}

void SGraphPinSMEventName::RefreshNameList(UStateNode* Node)
{
	NameList.Empty();

	if (Node)
	{
		TSet<FName> Events;
		Node->GetEmittedEvents(Events);

		for (auto& Name : Events)
		{
			TSharedPtr<FName> RowNameItem = MakeShareable(new FName(Name));
			NameList.Add(RowNameItem);
		}
	}
}

void SGraphPinSMEventName::RefreshNameList(TSubclassOf<UStateMachine> NewInterface)
{
	NameList.Empty();

	if (auto SMGC = Cast<UStateMachineBlueprintGeneratedClass>(NewInterface))
	{
		for (auto& EName : SMGC->GetEventSet(NAME_None))
		{
			TSharedPtr<FName> RowNameItem = MakeShareable(new FName(EName));
			NameList.Add(RowNameItem);
		}
	}
}
