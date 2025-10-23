#include "StateMachine/StateMachineInterface.h"
#include "StateMachine/StateMachine.h"

UStateMachineInterface::UStateMachineInterface()
{

}

bool UStateMachineInterface::VerifyNoCycles() const
{
	this->Parent.LoadSynchronous();
	UStateMachineInterface* CheckParent = this->Parent.Get();

	while (CheckParent)
	{
		if (CheckParent->Parent == this)
		{
			return false;
		}

		CheckParent = CheckParent->Parent.LoadSynchronous();
	}

	return true;
}

void UStateMachineInterface::SetParent(UStateMachineInterface* NewParent)
{
	this->Parent = NewParent;
}

void UStateMachineInterface::SetParent(TSoftObjectPtr<UStateMachineInterface> NewParent)
{
	this->Parent = NewParent;
}

bool UStateMachineInterface::HasEvent(FName InEvent) const
{
	bool bFound = this->GetEvents_Inner().Contains(InEvent);

	if (auto CheckParent = this->Parent.LoadSynchronous())
	{
		bFound = bFound || CheckParent->HasEvent(InEvent);
	}

	return bFound;
}

bool UStateMachineInterface::HasCallEvent(FName InEvent) const
{
	bool bFound = false;

	for (const auto& NodeClass : this->NodeEvents)
	{
		NodeClass.LoadSynchronous();

		if (NodeClass.IsValid())
		{
			TSet<FName> Notifies;
			CastChecked<UStateNode>(NodeClass->GetDefaultObject())->GetNotifies(Notifies);

			if (Notifies.Contains(InEvent))
			{
				bFound = true;
				break;
			}
		}
	}

	return bFound;
}

TSet<FName> UStateMachineInterface::GetEvents() const
{
	TSet<FName> Collect = this->GetEvents_Inner();

	if (auto CheckParent = this->Parent.LoadSynchronous())
	{
		Collect.Append(CheckParent->GetEvents());
	}

	return Collect;
}

TSet<FName> UStateMachineInterface::GetCallEvents() const
{
	TSet<FName> Collect = this->GetEvents();

	Collect.Append(this->GetNodeEvents());

	return Collect;
}

TSet<FName> UStateMachineInterface::GetNodeEvents() const
{
	TSet<FName> Collect;

	for (const auto& NodeClass : this->NodeEvents)
	{
		NodeClass.LoadSynchronous();

		if (NodeClass.IsValid())
		{
			CastChecked<UStateNode>(NodeClass->GetDefaultObject())->GetNotifies(Collect);
		}
	}

	return Collect;
}

FName UStateMachineInterface::EventToNamespaced(FName EventName) const
{
	FString AssetName = this->GetName();

	if (AssetName.StartsWith("SMI_"))
	{
		AssetName.LeftChopInline(4);
	}
	else if (AssetName.EndsWith("_SMI"))
	{
		AssetName.LeftChopInline(4);
	}

	AssetName.Append(".");
	AssetName.Append(EventName.ToString());

	return FName(AssetName);
}

void UStateMachineInterface::UpdateNamespacedEvents() const
{
	if (this->NamespacedEvents.Num() == 0)
	{
		for (const auto& EventName : this->Events)
		{
			this->NamespacedEvents.Add(this->EventToNamespaced(EventName.Key));
		}
	}
}

const TSet<FName>& UStateMachineInterface::GetEvents_Inner() const
{
	this->UpdateNamespacedEvents();

	return this->NamespacedEvents;
}

#if WITH_EDITOR
void UStateMachineInterface::PreEditChange(FProperty* Property)
{
	Super::PreEditChange(Property);

	if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStateMachineInterface, Parent))
	{
		this->ParentBackup = this->Parent;
	}
}

void UStateMachineInterface::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UStateMachineInterface, Parent))
	{
		if (!this->VerifyNoCycles())
		{
			this->Parent = ParentBackup;
			this->ParentBackup = nullptr;
		}
	}

	this->Events.KeySort([&](const FName& A, const FName& B) { return A.ToString() < B.ToString(); });

	this->NamespacedEvents.Empty();
	this->UpdateNamespacedEvents();	
}


#endif //WITH_EDITOR
