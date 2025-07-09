#include "Components/RPGSystem/RPGComponent.h"
#include "Components/RPGSystem/RPGProperty.h"

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

	/* For all default statuses, apply them immediately. */
	for (auto& Status : this->Statuses)
	{
		if (Status)
		{
			Status->Apply(this);
			if (Status->RequiresTick())
			{
				this->TickedStatuses.Add(Status);
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

void URPGComponent::TurnStart()
{
	for (auto& Status : this->Statuses)
	{
		if (IsValid(Status))
		{
			Status->TurnStart();
		}
	}
}

void URPGComponent::TurnEnd()
{
	for (auto& Status : this->Statuses)
	{
		if (IsValid(Status))
		{
			Status->TurnEnd();
		}
	}
}

void URPGComponent::PostLoad()
{
	Super::PostLoad();
	this->Validate();
}

void URPGComponent::Validate()
{
	/*
	for (TFieldIterator<FObjectProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{		
		FObjectProperty* f = *FIT;

		if (f->PropertyClass->IsChildOf<URPGProperty>())
		{
			TObjectPtr<UObject> Value;
			f->GetValue_InContainer(this, &Value);

			if (!IsValid(Value))
			{
				//auto NonNullValue = NewObject<URPGProperty>(this->GetOutermostObject(), f->PropertyClass);
				//f->SetValue_InContainer(this, NonNullValue);
			}
		}
	}
	*/
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

void URPGComponent::ApplyStatus(UStatus* Status)
{
	if (IsValid(Status) && !Status->IsOwned())
	{
		this->Statuses.Add(Status);
		Status->Apply(this);

		if (Status->RequiresTick())
		{
			this->TickedStatuses.Add(Status);
		}
	}
}

void URPGComponent::RemoveStatus(UStatus* Status)
{
	this->Statuses.Remove(Status);
	this->TickedStatuses.Remove(Status);
	Status->Remove();
}

void URPGComponent::StackStatus(TSubclassOf<UStatus> StatusType, int Quantity)
{
	for (auto& Status : this->Statuses)
	{
		Status->Stack(Quantity);
		return;
	}
}

UStatus* URPGComponent::GetStatus(TSubclassOf<UStatus> SClass, ESearchResult& Result)
{
	for (auto& Status : this->Statuses) {
		if (Status->IsA(SClass.Get())) {
			Result = ESearchResult::Found;
			return Status;
		}
	}

	Result = ESearchResult::NotFound;
	return nullptr;
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

#if WITH_EDITOR
void URPGComponent::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);
	this->Validate();
}

void URPGComponent::PostLinkerChange()
{
	Super::PostLinkerChange();
	this->Validate();
}
#endif

#pragma region Statuses

void UStatus::Apply(URPGComponent* Comp) {
	
	if (IsValid(Comp))
	{
		this->Owner = Comp;
		this->Stacks = 1;
		this->SetTimer();
		this->Apply_Inner();
	}
}

void UStatus::SetTimer()
{
	if (!this->IsPermanent())
	{
		this->Timer.Invalidate();
		this->GetWorld()->GetTimerManager().SetTimer(
			this->Timer,
			this,
			&UStatus::OnDurationExpired,
			this->Duration,
			false);
	}
}

void UStatus::OnDurationExpired()
{
	this->Owner.Get()->RemoveStatus(this);
}

void UStatus::Stack(int Quantity)
{
	this->Stacks += Quantity;

	if (this->bRefreshOnStack)
	{
		this->SetTimer();
	}

	this->Stack_Inner(Quantity);
}

void UStatus::Remove() {	
	this->Remove_Inner();
	this->Owner = nullptr;

	this->Timer.Invalidate();
}

void UStatus::Detach()
{
	if (this->Owner.IsValid())
	{
		this->Owner.Get()->RemoveStatus(this);
	}
}

URPGComponent* UStatus::GetOwner() const
{
	if (this->Owner.IsValid())
	{
		return this->Owner.Get();
	}
	else
	{
		return nullptr;
	}
}

UWorld* UStatus::GetWorld() const
{
	if (this->Owner.IsValid())
	{
		if (auto Actor = this->Owner.Get()->GetOwner())
		{
			return Actor->GetWorld();
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

#pragma endregion
