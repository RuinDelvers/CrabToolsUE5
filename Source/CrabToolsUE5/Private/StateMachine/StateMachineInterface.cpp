#include "StateMachine/StateMachineInterface.h"
#include "StateMachine/StateMachine.h"

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

bool UStateMachineInterface::HasEvent(FName EName) const
{
	bool bFound = this->Events.Contains(EName);

	if (auto CheckParent = this->Parent.LoadSynchronous())
	{
		bFound = bFound || CheckParent->HasEvent(EName);
	}

	return bFound;
}

TSet<FName> UStateMachineInterface::GetEvents() const
{
	TSet<FName> Collect;

	for (auto& Name : this->Events)
	{
		Collect.Add(Name.Key);
	}

	if (auto CheckParent = this->Parent.LoadSynchronous())
	{
		Collect.Append(CheckParent->GetEvents());
	}

	return Collect;
}

TSet<FName> UStateMachineInterface::GetCallEvents() const
{
	TSet<FName> Collect = this->GetEvents();

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

TSet<FName> UStateMachineInterface::GetStates() const
{
	TSet<FName> Collect;

	for (auto& Name : this->States)
	{
		Collect.Add(Name.Key);
	}

	if (auto CheckParent = this->Parent.LoadSynchronous())
	{
		Collect.Append(CheckParent->GetStates());
	}

	return Collect;
}

TSet<FName> UStateMachineInterface::GetSubMachines() const
{
	TSet<FName> Collect;

	for (auto& Name : this->SubMachines)
	{
		Collect.Add(Name.Key);
	}

	if (auto CheckParent = this->Parent.LoadSynchronous())
	{
		Collect.Append(CheckParent->GetSubMachines());
	}

	return Collect;
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
}
#endif //WITH_EDITOR
