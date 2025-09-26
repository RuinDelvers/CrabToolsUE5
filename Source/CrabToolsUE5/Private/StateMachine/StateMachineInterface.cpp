#include "StateMachine/StateMachineInterface.h"
#include "StateMachine/StateMachine.h"
#include "GameplayTagsManager.h"
#include "UObject/ObjectSaveContext.h"



UStateMachineInterface::UStateMachineInterface()
{
	//this->RowStruct == FGameplayTagTableRow::StaticStruct();
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

bool UStateMachineInterface::HasEvent(FName EName) const
{
	bool bFound = this->Events.Contains(EName);

	if (auto CheckParent = this->Parent.LoadSynchronous())
	{
		bFound = bFound || CheckParent->HasEvent(EName);
	}

	return bFound;
}

bool UStateMachineInterface::HasCallEvent(FName EName) const
{
	bool bFound = false;

	for (const auto& NodeClass : this->NodeEvents)
	{
		NodeClass.LoadSynchronous();

		if (NodeClass.IsValid())
		{
			TSet<FName> Notifies;
			CastChecked<UStateNode>(NodeClass->GetDefaultObject())->GetNotifies(Notifies);

			if (Notifies.Contains(EName))
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

FName UStateMachineInterface::GenerateGameplayTagFrom(FName EventName, const FSMIData& Data) const
{
	FString AssetName = this->GetName();

	if (AssetName.StartsWith("SMI_"))
	{
		AssetName = AssetName.RightChop(4);
	}
	else if (AssetName.EndsWith("_SMI"))
	{
		AssetName = AssetName.LeftChop(AssetName.Len() - 4);
	}

	AssetName.Append(".");
	AssetName.Append(EventName.ToString());

	return FName(AssetName);
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

	this->Events.KeySort([&](const FName& A, const FName& B) { return A.FastLess(B); });
}

void UStateMachineInterface::PreSaveRoot(FObjectPreSaveRootContext ObjectSaveContext)
{
	/*
	for (const auto& RowName : this->GetRowNames())
	{
		if (!this->HasEvent(RowName))
		{
			this->RemoveRow(RowName);
		}
	}

	for (const auto& Event : this->Events)
	{
		if (!this->FindRowUnchecked(Event.Key))
		{
			this->AddEventToTable(Event.Key, Event.Value);
		}
	}

	for (const auto& NodeClas : this->NodeEvents)
	{
		for (const auto& NodeClass : this->NodeEvents)
		{
			NodeClass.LoadSynchronous();

			if (NodeClass.IsValid())
			{
				//TSet<FName> NodeEventNames;
				//CastChecked<UStateNode>(NodeClass->GetDefaultObject())->GetNotifies(NodeEventNames);

				// TODO Add these but need to update how nodes define their events.
			}
		}
	}
	*/

	Super::PreSaveRoot(ObjectSaveContext);
}

void UStateMachineInterface::AddEventToTable(FName EName, const FSMIData& Data)
{
	FName TagName = this->GenerateGameplayTagFrom(EName, Data);
	FGameplayTagTableRow GPData;
	FString TagValue = TagName.ToString();
	TagValue.InsertAt(0, "StateMachine.Events.");

	GPData.Tag = FName(TagValue);

	//this->AddRow(TagName, GPData);
}

#endif //WITH_EDITOR
