#include "Components/RPGSystem/RPGComponent.h"
#include "Components/RPGSystem/RPGProperty.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"



#define LOCTEXT_NAMESPACE "RPGComponent"

#pragma region Component Code

URPGComponent::URPGComponent(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
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
			else
			{
				FMessageLog Log("RPGSystem");
				Log.Error(FText::FormatNamed(
					LOCTEXT("NullPropertyError", "RPGProperty {Prop} was null for actor {Actor}."),
					TEXT("Prop"), FText::FromString(f->GetName()),
					TEXT(""), FText::FromString(this->GetOwner()->GetName())));
			}
		}
	}

	Super::BeginPlay();
}

void URPGComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		
	for (auto& Status : this->TickedStatuses) {
		if (IsValid(Status)) {
			Status->Tick(DeltaTime);
		}
	}
}

UStatus* URPGComponent::MakeStatus(TSubclassOf<UStatus> StatusClass)
{
	auto Status = NewObject<UStatus>(this, StatusClass);
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

	this->OnTurnStart.Broadcast(this);
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

	this->OnTurnEnd.Broadcast(this);
}

TArray<FString> URPGComponent::GetRPGPropertyNames(TSubclassOf<URPGProperty> Props, bool bRecurse) const
{
	return this->GetRPGPropertyNames(Props.Get(), bRecurse, false);
}
TArray<FString> URPGComponent::GetRPGPropertyNamesFromInterface(TSubclassOf<UInterface> Props, bool bRecurse) const
{
	return this->GetRPGPropertyNames(Props.Get(), bRecurse, true);
}


TArray<FString> URPGComponent::GetRPGPropertyNames(UClass* Props, bool bRecurse, bool bIsInterface) const
{
	TArray<FString> Names;

	for (TFieldIterator<FObjectProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FObjectProperty* f = *FIT;

		if (bIsInterface ? f->PropertyClass->ImplementsInterface(Props) : f->PropertyClass->IsChildOf(Props))
		{
			Names.Add(f->GetName());
		}
	}
	
	if (bRecurse)
	{
		// Search actor owners for properties. This is for native classes.
		if (auto ActorOuter = Cast<AActor>(this->GetOuter()))
		{
			for (TFieldIterator<FObjectProperty> FIT(ActorOuter->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
			{
				FObjectProperty* f = *FIT;
				TObjectPtr<UObject> Value;

				if (f->PropertyClass->IsChildOf<URPGComponent>())
				{
					f->GetValue_InContainer(this->GetOuter(), &Value);

					if (auto Prop = Cast<URPGComponent>(Value))
					{
						if (Prop != this)
						{
							auto OtherNames = Prop->GetRPGPropertyNames(Props, false, bIsInterface);

							for (int i = 0; i < OtherNames.Num(); ++i)
							{
								FString Base = f->GetName();
								Base.PathAppend(*OtherNames[i], OtherNames.Num());
								OtherNames[i] = Base;
								Names.Add(Base);
							}
						}
					}
				}
			}
		}

		// Search blueprint classes for other components. This is for blueprint classes.
		if (auto BPClass = Cast<UBlueprintGeneratedClass>(this->GetOuter()))
		{
			if (BPClass->SimpleConstructionScript)
			{
				for (const auto& Node : BPClass->SimpleConstructionScript->GetAllNodes())
				{
					if (Node)
					{
						if (auto RPGComp = Cast<URPGComponent>(Node->ComponentTemplate))
						{
							if (RPGComp != this)
							{
								auto OtherNames = RPGComp->GetRPGPropertyNames(Props, false);

								for (int i = 0; i < OtherNames.Num(); ++i)
								{
									FString Base = Node->GetVariableName().ToString();
									Base.Append("/");
									Base.Append(OtherNames[i]);
									Names.Add(Base);
								}
							}
						}
					}
				}
			}
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
	
}

void URPGComponent::ApplyStatus(UStatus* Status)
{
	if (!IsValid(Status) || Status->GetOwner() != this || Status->IsAttached())
	{
		return;
	}

	Status->OnExpired.AddDynamic(this, &URPGComponent::OnStatusExpiredCallback);

	if (auto Data = this->Statuses.Find(Status->GetStatusGroup()))
	{
		Data->Instances.Add(Status);
	}
	else
	{
		FStatusData NewData;
		NewData.Instances.Add(Status);

		this->Statuses.Add(Status->GetStatusGroup(), NewData);
	}

	if (Status)
	{
		Status->Apply();

		if (Status->RequiresTick())
		{
			this->TickedStatuses.Add(Status);
		}

		this->OnStatusAdded.Broadcast(Status);
		Status->OnAdded.Broadcast(Status);
	}
}

const FStatusData* URPGComponent::GetStatusGroupData(FGameplayTag Group) const
{
	return this->Statuses.Find(Group);
}

UStatus* URPGComponent::GetStatusInstanceFromGroup(FGameplayTag Group) const
{
	if (this->Statuses.Contains(Group))
	{
		return this->Statuses[Group].Instances[0];
	}
	else
	{
		return nullptr;
	}
}

void URPGComponent::OnStatusExpiredCallback(UStatus* Status)
{
	this->RemoveStatusInstance(Status);
}

void URPGComponent::RemoveStatusInstance(UStatus* Status)
{
	auto Group = Status->GetStatusGroup();

	if (auto Data = this->Statuses.Find(Group))
	{
		if (Data->Instances.Contains(Status))
		{
			Data->Instances.Remove(Status);
			Status->Remove();

			if (Data->Instances.Num() == 0)
			{
				this->Statuses.Remove(Group);
			}

			this->OnStatusRemoved.Broadcast(Status);
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
	if (StatusID.IsValid())
	{
		if (auto Data = this->Statuses.Find(StatusID))
		{
			Found.Append(Data->Instances);
		}
	}
	else
	{
		for (const auto& Status : this->Statuses)
		{
			Found.Append(Status.Value.Instances);
		}
	}
}

bool URPGComponent::HasStatus(FGameplayTag StatusID) const
{
	return this->Statuses.Contains(StatusID);
}

URPGProperty* URPGComponent::FindRPGPropertyByName(FName Ref, bool bRecurse) const
{
	if (Ref.IsNone()) { return nullptr; }

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
	else if (bRecurse)
	{
		FString RefString = Ref.ToString();
		FString CompName;
		FString PropName;

		if (RefString.Split("/", &CompName, &PropName))
		{
			if (auto Actor = this->GetOwner())
			{
				if (auto Property = CastField<FObjectProperty>(Actor->GetClass()->FindPropertyByName(FName(CompName))))
				{
					if (Property->PropertyClass->IsChildOf<URPGComponent>())
					{
						TObjectPtr<UObject> Value;
						Property->GetValue_InContainer(this, &Value);

						if (auto Comp = Cast<URPGComponent>(Value))
						{
							return Comp->FindRPGPropertyByName(FName(PropName), false);
						}
					}
				}
			}
		}		
	}

	return nullptr;
}

URPGProperty* URPGComponent::FindDefaultRPGPropertyByName(FName Ref) const
{
	return this->FindRPGPropertyByName(Ref, false);
}

void URPGComponent::PauseStatus()
{
	for (const auto& Data : this->Statuses)
	{
		for (const auto& Status : Data.Value.Instances)
		{
			Status->PauseTimer();
		}		
	}
	this->SetComponentTickEnabled(false);
}

void URPGComponent::UnpauseStatus()
{
	for (const auto& Data : this->Statuses)
	{
		for (const auto& Status : Data.Value.Instances)
		{
			Status->UnpauseTimer();
		}
	}
	this->SetComponentTickEnabled(true);
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

void UStatus::Attach()
{
	if (!this->bAttached)
	{
		this->GetOwner()->ApplyStatus(this);
		this->bAttached = true;
	}
}

bool UStatus::IsAttached() const
{
	return this->bAttached;
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
	this->OnRemoved.Broadcast(this);
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

#undef LOCTEXT_NAMESPACE
