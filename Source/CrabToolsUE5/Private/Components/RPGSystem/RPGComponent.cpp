#include "Components/RPGSystem/RPGComponent.h"
#include "Components/RPGSystem/RPGProperty.h"
#include "Settings/CrabTools_Settings.h"

#pragma region Component Code
URPGComponent::URPGComponent(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer) {
	this->bWantsInitializeComponent = true;
	this->bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void URPGComponent::BeginPlay()
{
	for (TFieldIterator<FObjectProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FObjectProperty* f = *FIT;
		TObjectPtr<UObject> Value;

		if (f->PropertyClass->IsChildOf<URPGProperty>())
		{
			f->GetValue_InContainer(this, &Value);

			if (auto Prop = Cast<URPGProperty>(Value))
			{
				Prop->Initialize(this);
			}
		}
	}

	Super::BeginPlay();
}

void URPGComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		
	for (auto& Status : this->TickedStatuses) {
		if (IsValid(Status)) {
			Status->Tick(DeltaTime);
		}
	}
}

UStatus* URPGComponent::MakeStatus(TSubclassOf<UStatus> StatusClass, FGameplayTag StatusID)
{
	auto Status = NewObject<UStatus>(this, StatusClass);
	Status->StatusID = StatusID;
	return Status;
}

void URPGComponent::TurnStart()
{
	for (auto& StatusData : this->Statuses)
	{
		for (auto& Status : StatusData.Value.Instances)
		{
			Status->TurnStart();
		}
	}
}

void URPGComponent::TurnEnd()
{
	for (auto& StatusData : this->Statuses)
	{
		for (auto& Status : StatusData.Value.Instances)
		{
			Status->TurnEnd();
		}
	}
}

TArray<FString> URPGComponent::GetRPGPropertyNames(TSubclassOf<URPGProperty> Props) const
{
	TArray<FString> Names;

	for (TFieldIterator<FObjectProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FObjectProperty* f = *FIT;

		if (f->PropertyClass->IsChildOf(Props))
		{
			Names.Add(f->GetName());
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
	
}

void URPGComponent::ApplyStatus(UStatus* Status, int Stacks)
{
	if (!IsValid(Status) || Status->GetOwner() != this)
	{
		return;
	}

	bool bAddedStatus = false;
	auto StackType = Status->GetStackType();

	bAddedStatus = true;

	if (auto Data = this->Statuses.Find(Status->StatusID))
	{
		switch (StackType)
		{
		case EStatusStackType::ADD:
			for (int i = 0; i < Stacks; ++i)
			{
				Status->Stack(1);
				Data->Instances.Add(Status);
				this->ObjTagMap.Add(Status, Status->StatusID);
				Status->OnExpired.AddDynamic(this, &URPGComponent::OnStatusExpired);
				Status->Apply();
			}
			break;
		case EStatusStackType::REFRESH:
			Data->Instances[0]->Refresh();
			break;
		case EStatusStackType::REFRESH_SUM:
			Data->Instances[0]->RefreshSum();
			break;
		case EStatusStackType::STACK:
			Data->Instances[0]->Stack(Stacks);
			break;
		case EStatusStackType::UNIQUE:
			break;
		}
	}
	else
	{
		Status->Stack(Stacks);
		FStatusData NewData;
		NewData.Instances.Add(Status);
		this->ObjTagMap.Add(Status, Status->StatusID);
		Status->OnExpired.AddDynamic(this, &URPGComponent::OnStatusExpired);
		Status->Apply();

		this->Statuses.Add(Status->StatusID, NewData);
	}

	if (Status)
	{
		if (Status->RequiresTick())
		{
			this->TickedStatuses.Add(Status);
		}
	}
}

void URPGComponent::OnStatusExpired(UStatus* Status)
{
	this->RemoveStatusInstance(Status);
}

void URPGComponent::RemoveStatusInstance(UStatus* Status)
{
	if (auto Tag = this->ObjTagMap.Find(Status))
	{
		this->ObjTagMap.Remove(Status);

		if (auto Data = this->Statuses.Find(*Tag))
		{
			if (Data->Instances.Contains(Status))
			{
				Data->Instances.Remove(Status);
				Status->Remove();

				if (Data->Instances.Num() == 0)
				{
					this->Statuses.Remove(*Tag);
				}
			}
		}
	}
}

void URPGComponent::RemoveStatus(FGameplayTag StatusID, bool bAllInstances)
{
	if (auto Data = this->Statuses.Find(StatusID))
	{
		if (bAllInstances)
		{
			for (const auto& Instance : Data->Instances)
			{
				this->TickedStatuses.Remove(Instance);
				Instance->Remove();
			}
			this->Statuses.Remove(StatusID);

		}
		else
		{			
			auto Removed = Data->Instances.Pop();

		}
	}	
}

void URPGComponent::GetStatus(FGameplayTag StatusID, TArray<UStatus*>& Found)
{
	if (auto Data = this->Statuses.Find(StatusID))
	{
		Found.Append(Data->Instances);
	}
}

URPGProperty* URPGComponent::FindRPGPropertyByName(FName Ref) const
{
	auto Prop = this->GetClass()->FindPropertyByName(Ref);

	if (Prop)
	{
		if (auto ObjProp = CastField<FObjectProperty>(Prop))
		{
			if (ObjProp->PropertyClass->IsChildOf<URPGProperty>())
			{
				TObjectPtr<UObject> Value;
				ObjProp->GetValue_InContainer(this, &Value);

				return Cast<URPGProperty>(Value);
			}
		}
	}

	return nullptr;
}

void URPGComponent::PauseStatus()
{
	for (const auto& Data : this->ObjTagMap)
	{
		Data.Key->PauseTimer();
		this->SetComponentTickEnabled(false);
	}
}

void URPGComponent::UnpauseStatus()
{
	for (const auto& Data : this->ObjTagMap)
	{
		Data.Key->UnpauseTimer();
		this->SetComponentTickEnabled(true);
	}
}

#pragma region Statuses

URPGComponent* UStatus::GetOwner() const
{
	return CastChecked<URPGComponent>(this->GetOuter());
}

float UStatus::GetRemainingTime() const
{
	return this->GetWorld()->GetTimerManager().GetTimerRemaining(this->Timer);
}

void UStatus::Refresh()
{
	this->SetTimer();
}

void UStatus::RefreshSum()
{
	this->AddTimer(this->GetDuration());
}

void UStatus::PauseTimer()
{
	this->GetWorld()->GetTimerManager().PauseTimer(this->Timer);
}

void UStatus::UnpauseTimer()
{
	this->GetWorld()->GetTimerManager().UnPauseTimer(this->Timer);
}

void UStatus::SetTimer()
{
	this->Timer.Invalidate();
	this->GetWorld()->GetTimerManager().SetTimer(
		this->Timer,
		this,
		&UStatus::OnDurationExpired,
		this->GetDuration(),
		false);
}

void UStatus::AddTimer(float Amount)
{
	this->GetWorld()->GetTimerManager().SetTimer(
		this->Timer,
		this,
		&UStatus::OnDurationExpired,
		this->GetRemainingTime() + Amount,
		false);
}

void UStatus::OnDurationExpired()
{
	this->OnExpired.Broadcast(this);
}

void UStatus::Stack(int Quantity)
{
	this->Stacks += Quantity;
	this->Stack_Inner(Quantity);
}

void UStatus::Attach(int InitStacks)
{
	if (!this->bAttached)
	{
		this->bAttached = true;
		this->GetOwner()->ApplyStatus(this, InitStacks);
	}
}

void UStatus::Detach()
{
	this->GetOwner()->RemoveStatusInstance(this);
}

void UStatus::Apply()
{
	this->Apply_Inner();
	this->SetTimer();
}

void UStatus::Remove()
{
	this->Timer.Invalidate();
	this->Remove_Inner();
}

UWorld* UStatus::GetWorld() const
{
	if (auto Owner = this->GetOwner())
	{
		return Owner->GetWorld();
	}
	else
	{
		return nullptr;
	}
}

#pragma endregion
