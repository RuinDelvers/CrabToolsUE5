#include "Widgets/SGraphPinSMEventName.h"

#include "Containers/Array.h"
#include "Engine/DataTable.h"
#include "HAL/PlatformCrt.h"
#include "StateMachine/StateMachineInterface.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/StateMachine.h"
#include "Templates/SharedPointer.h"
#include "UObject/NameTypes.h"

class UEdGraphPin;

void SGraphPinSMEventName::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj, UStateMachineInterface* InInterface, TSubclassOf<UStateMachine> SMClass)
{
	if (IsValid(InInterface))
	{
		RefreshNameList(InInterface);
	}
	else if (SMClass)
	{

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

void SGraphPinSMEventName::RefreshNameList(TSubclassOf<UStateMachine> NewInterface)
{
	NameList.Empty();

	if (auto SMGC = Cast<UStateMachineBlueprintGeneratedClass>(NewInterface))
	{
		for (auto& EName : SMGC->EventSet)
		{
			TSharedPtr<FName> RowNameItem = MakeShareable(new FName(EName));
			NameList.Add(RowNameItem);
		}
	}
}
