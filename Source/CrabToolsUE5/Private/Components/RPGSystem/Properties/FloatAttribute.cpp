#include "Components/RPGSystem/Properties/FloatAttribute.h"
#include "Components/RPGSystem/RPGComponent.h"

void UBaseFloatAttribute::Initialize_Inner_Implementation()
{
	this->Refresh();
}

float UBaseFloatAttribute::GetValue() const
{
	return this->CompValue;
}

void UBaseFloatAttribute::Operate(UFloatOperator* Op)
{
	if (IsValid(Op))
	{
		this->Operators.Add(Op);
		this->Refresh();
	}
}

void UBaseFloatAttribute::UnOperate(UFloatOperator* Op)
{
	if (IsValid(Op))
	{
		this->Operators.Remove(Op);
		this->Refresh();
	}
}

FText UBaseFloatAttribute::GetDisplayText_Implementation() const
{
	return FText::AsNumber(this->GetValue());
}

void UBaseFloatAttribute::Refresh()
{
	auto Value = this->GetBaseValue();

	for (auto& Op : this->Operators)
	{
		Value = Op->Operate(Value);
	}

	this->CompValue = Value;

	this->OnPropertyChanged.Broadcast(this);
}


void UFloatAttribute::SetBaseValue(float UValue)
{
	this->BaseValue = UValue;
	this->Refresh();
}

TSubclassOf<URPGSetter> UBaseFloatAttribute::GetSetter_Implementation() const
{
	return UFloatPropertySetter::StaticClass();
}

TSubclassOf<URPGCompare> UBaseFloatAttribute::GetCompare_Implementation() const
{
	return UFloatPropertyCompare::StaticClass();;
}


void UFloatPropertySetter::Initialize_Inner_Implementation()
{
	if (this->bInline)
	{
		this->SourceProperty = this->Component->FindRPGPropertyByName(this->SourcePropertyName);
	}
}

void UFloatPropertySetter::ApplyValue_Implementation()
{
	if (this->bInline)
	{
		IFloatRPGProperty::Execute_SetFloatValue(this->Property, this->Value);
	}
	else
	{
		IFloatRPGProperty::Execute_SetFloatValue(
			this->Property,
			IFloatRPGProperty::Execute_GetFloatValue(this->SourceProperty));
	}
}

void UFloatPropertyCompare::Initialize_Inner_Implementation()
{
	if (this->bInline)
	{
		this->CompareProperty = this->Component->FindRPGPropertyByName(this->ComparePropertyName);
	}
}

bool UFloatPropertyCompare::Compare_Implementation()
{
	if (this->bInline)
	{
		return NumericComparison::Compare(
			this->Comparison,
			IFloatRPGProperty::Execute_GetFloatValue(this->Property),
			this->Value);
	}
	else
	{
		return NumericComparison::Compare(
			this->Comparison,
			IFloatRPGProperty::Execute_GetFloatValue(this->Property),
			IFloatRPGProperty::Execute_GetFloatValue(this->CompareProperty));
	}
}