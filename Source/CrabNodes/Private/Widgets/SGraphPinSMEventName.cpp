#include "Widgets/SGraphPinSMEventName.h"

#include "Containers/Array.h"
#include "Kismet/K2Node_EmitEventBase.h"
#include "Templates/SharedPointer.h"
#include "UObject/NameTypes.h"

class UEdGraphPin;

void SGraphPinSMEventName::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj, UK2Node_EmitEventBase* Node)
{
	this->RefreshNameList(Node->GetEventSet());
	Node->OnEventSetChanged.AddSP(this, &SGraphPinSMEventName::RefreshNameList);
	SGraphPinNameList::Construct(SGraphPinNameList::FArguments(), InGraphPinObj, NameList);
}

void SGraphPinSMEventName::RefreshNameList(const TSet<FName>& Names)
{
	NameList.Empty();

	for (auto& Name : Names)
	{
		TSharedPtr<FName> RowNameItem = MakeShareable(new FName(Name));
		NameList.Add(RowNameItem);
	}
}

SGraphPinSMEventName::SGraphPinSMEventName()
{
}

SGraphPinSMEventName::~SGraphPinSMEventName()
{
}
