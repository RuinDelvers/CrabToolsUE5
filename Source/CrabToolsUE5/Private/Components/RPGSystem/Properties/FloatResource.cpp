#include "Components/RPGSystem/Properties/FloatResource.h"
#include "Components/RPGSystem/RPGComponent.h"
#include "Utils/UtilsLibrary.h"

UFloatResource::UFloatResource()
{
	
}

void UFloatResource::Initialize_Inner_Implementation()
{
	if (this->Minimum == nullptr)
	{
		if (this->MinimumRef.IsNone())
		{
			this->Minimum = NewObject<UMinFloatAttribute>(this->GetOwner());
		}
		else
		{
			this->Minimum = CastChecked<UBaseFloatAttribute>(this->GetOwner()->FindRPGPropertyByName(this->MinimumRef));
		}
	}

	if (this->Maximum == nullptr)
	{
		if (this->MaximumRef.IsNone())
		{
			this->Maximum = NewObject<UMaxFloatAttribute>(this->GetOwner());
		}
		else
		{
			this->Maximum = CastChecked<UBaseFloatAttribute>(this->GetOwner()->FindRPGPropertyByName(this->MaximumRef));
		}
	}

	this->Minimum->Initialize(this->GetOwner());
	this->Maximum->Initialize(this->GetOwner());

	this->Minimum->OnAttributeChanged.AddDynamic(this, &UFloatResource::OnAttributeChanged);
	this->Maximum->OnAttributeChanged.AddDynamic(this, &UFloatResource::OnAttributeChanged);
}

void UFloatResource::SetValue(float UValue)
{
	auto NewValue = FMath::Clamp(UValue, this->GetMin(), this->GetMax());

	if (NewValue != this->Value)
	{
		this->Value = NewValue;
		this->OnResourceChanged.Broadcast(this);
	}
}

float UFloatResource::GetPercent() const
{
	float Min = this->GetMin();
	float Max = this->GetMax();
	bool IsMinFinite = FMath::IsFinite(Min);
	bool IsMaxFinite = FMath::IsFinite(Max);

	if (IsMinFinite && IsMaxFinite)
	{
		return FMath::Clamp(this->GetValue() / (Max - Min), 0.0, 1.0);
	}
	else
	{
		return 0.0;
	}
}

void UFloatResource::SetMinProp(URPGProperty* Prop)
{
	this->Minimum = CastChecked<UBaseFloatAttribute>(Prop);
}

void UFloatResource::SetMaxProp(URPGProperty* Prop)
{
	this->Maximum = CastChecked<UBaseFloatAttribute>(Prop);
}

void UFloatResource::Refresh()
{
	auto NewValue = FMath::Clamp(this->Value, this->GetMin(), this->GetMax());

	if (NewValue != this->Value)
	{
		this->Value = NewValue;
		this->OnResourceChanged.Broadcast(this);
	}
}

void UFloatResource::UseResourceInt_Implementation(int Amount)
{
	this->SetValue(this->GetValue() - Amount);
}

void UFloatResource::UseResourceFloat_Implementation(float Amount)
{
	this->SetValue(this->GetValue() - Amount);
}

bool UFloatResource::HasResourceInt_Implementation(int Compare) const
{
	return Compare <= this->GetValue();
}

bool UFloatResource::HasResourceFloat_Implementation(float Compare) const
{
	return Compare <= this->GetValue();
}

FText UFloatResource::GetDisplayText_Implementation() const
{
	return FText::AsNumber(this->GetValue());
}

void UFloatResource::OnAttributeChanged(UBaseFloatAttribute* Attr)
{
	this->Refresh();
}

TArray<FString> UFloatResource::GetAttributeOptions() const
{

	if (auto Outer = UtilsFunctions::GetOuterAs<URPGComponent>(this))
	{
		return Outer->GetRPGPropertyNames(UFloatAttribute::StaticClass());
	}

	return {};
}
