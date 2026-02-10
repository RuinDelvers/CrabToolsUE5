#include "Components/RPGSystem/Properties/SemaphoreProperty.h"
#include "Components/RPGSystem/RPGComponent.h"

void USemaphoreProperty::AddLock(UObject* NewLock)
{
	bool bOldValue = this->LockValue();

	this->SemaphoreLocks.Add(NewLock);

	if (this->LockValue() != bOldValue)
	{
		this->OnPropertyChanged.Broadcast(this);
	}
}

void USemaphoreProperty::RemoveLock(UObject* NewLock)
{
	bool bOldValue = this->LockValue();

	this->SemaphoreLocks.Remove(NewLock);

	if (this->LockValue() != bOldValue)
	{
		this->OnPropertyChanged.Broadcast(this);
	}
}

bool USemaphoreProperty::LockValue() const
{
	return this->SemaphoreLocks.Num() == 0;
}

TSubclassOf<URPGSetter> USemaphoreProperty::GetSetter_Implementation() const
{
	return USemaphorePropertySetter::StaticClass();
}

TSubclassOf<URPGCompare> USemaphoreProperty::GetCompare_Implementation() const
{
	return USemaphorePropertyCompare::StaticClass();
}

bool USemaphorePropertyCompare::Compare_Implementation()
{
	return CastChecked<USemaphoreProperty>(this->Property)->LockValue() == CastChecked<USemaphoreProperty>(this->CompareProperty)->LockValue();
}

void USemaphorePropertyCompare::Initialize_Inner_Implementation()
{
	this->CompareProperty = this->Component->FindRPGPropertyByName(this->ComparePropertyName);
}

void USemaphorePropertySetter::ApplyValue_Implementation()
{
	auto Prop = CastChecked<USemaphoreProperty>(this->Property);

	Prop->AddLock(this);
}

void USemaphorePropertySetter::UnapplyValue_Implementation()
{
	auto Prop = CastChecked<USemaphoreProperty>(this->Property);

	Prop->RemoveLock(this);
}
