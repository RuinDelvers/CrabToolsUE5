#include "Components/RPGSystem/Properties/IntResource.h"
#include "Components/RPGSystem/RPGComponent.h"
#include "Utils/UtilsLibrary.h"

UIntResource::UIntResource()
{
	
}

void UIntResource::Initialize_Inner_Implementation()
{
	if (this->Minimum == nullptr)
	{
		if (this->MinimumRef.IsNone())
		{
			this->Minimum = NewObject<UMinIntAttribute>(this->GetOwner());
		}
		else
		{
			this->Minimum = CastChecked<UBaseIntAttribute>(this->GetOwner()->FindRPGPropertyByName(this->MinimumRef));
		}
	}

	if (this->Maximum == nullptr)
	{
		if (this->MaximumRef.IsNone())
		{
			this->Maximum = NewObject<UMaxIntAttribute>(this->GetOwner());
		}
		else
		{
			this->Maximum = CastChecked<UBaseIntAttribute>(this->GetOwner()->FindRPGPropertyByName(this->MaximumRef));
		}
	}

	this->Minimum->Initialize(this->GetOwner());
	this->Maximum->Initialize(this->GetOwner());

	this->Minimum->OnAttributeChanged.AddDynamic(this, &UIntResource::OnAttributeChanged);
	this->Maximum->OnAttributeChanged.AddDynamic(this, &UIntResource::OnAttributeChanged);
}

FText UIntResource::GetDisplayText_Implementation() const
{
	return FText::AsNumber(this->GetValue());
}

void UIntResource::SetValue(int UValue)
{
	auto NewValue = FMath::Clamp(UValue, this->GetMin(), this->GetMax());

	if (NewValue != this->Value)
	{
		this->Value = NewValue;
		this->OnResourceChanged.Broadcast(this);
	}
}

void UIntResource::Increment()
{
	this->SetValue(this->GetValue() + 1);
}

void UIntResource::Decrement()
{
	this->SetValue(this->GetValue() - 1);
}

void UIntResource::UseResourceInt_Implementation(int Amount)
{
	this->SetValue(this->GetValue() - Amount);
}

void UIntResource::UseResourceFloat_Implementation(float Amount)
{
	this->SetValue(this->GetValue() - Amount);
}

bool UIntResource::HasResourceInt_Implementation(int Compare) const
{
	return Compare <= this->GetValue();
}

bool UIntResource::HasResourceFloat_Implementation(float Compare) const
{
	return Compare <= this->GetValue();
}

void UIntResource::SetMinProp(URPGProperty* Prop)
{
	this->Minimum = CastChecked<UBaseIntAttribute>(Prop);
}

void UIntResource::SetMaxProp(URPGProperty* Prop)
{
	this->Maximum = CastChecked<UBaseIntAttribute>(Prop);
}

float UIntResource::GetPercent() const
{
	int Min = this->GetMin();
	int Max = this->GetMax();
	int Diff = Max - Min;

	if (Diff == 0)
	{
		return 0.0;
	}
	else
	{
		float MinFloat = (float)Min;
		float MaxFloat = (float)Max;

		return FMath::Clamp(this->GetValue() / (MaxFloat - MinFloat), 0.0, 1.0);
	}
}

void UIntResource::Refresh()
{
	auto NewValue = FMath::Clamp(this->Value, this->GetMin(), this->GetMax());

	if (NewValue != this->Value)
	{
		this->Value = NewValue;
		this->OnResourceChanged.Broadcast(this);
	}	
}

void UIntResource::OnAttributeChanged(UBaseIntAttribute* Attr)
{
	this->Refresh();
}

TArray<FString> UIntResource::GetAttributeOptions() const
{
	if (auto Outer = UtilsFunctions::GetOuterAs<URPGComponent>(this))
	{
		return Outer->GetRPGPropertyNames(UIntAttribute::StaticClass());
	}

	return {};
}
