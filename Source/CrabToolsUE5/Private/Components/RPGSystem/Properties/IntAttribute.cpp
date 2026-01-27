#include "Components/RPGSystem/Properties/IntAttribute.h"
#include "Components/RPGSystem/RPGComponent.h"

void UBaseIntAttribute::Initialize_Inner_Implementation()
{	
	this->Refresh();
}

FText UBaseIntAttribute::GetDisplayText_Implementation() const
{
	return FText::AsNumber(this->GetValue());
}

int UBaseIntAttribute::GetValue() const
{
	return this->CompValue;
}

void UBaseIntAttribute::Operate(UIntOperator* Op)
{
	if (IsValid(Op))
	{
		this->Operators.Add(Op);
		Op->AddApplied(this);
		this->Refresh();		
	}
}

void UBaseIntAttribute::UnOperate(UIntOperator* Op)
{
	if (IsValid(Op))
	{
		int Amt = this->Operators.Remove(Op);
		Op->RemoveApplied(this);
		this->Refresh();
	}
}

void UBaseIntAttribute::Refresh()
{
	auto Value = this->GetBaseValue();

	for (auto& Op : this->Operators)
	{
		Value = Op->Operate(Value);
	}

	this->CompValue = Value;

	this->OnPropertyChanged.Broadcast(this);
}

TSubclassOf<URPGSetter> UBaseIntAttribute::GetSetter_Implementation() const
{
	return UIntegerPropertySetter::StaticClass();
}

TSubclassOf<URPGCompare> UBaseIntAttribute::GetCompare_Implementation() const
{
	return UIntegerPropertyCompare::StaticClass();;
}


void UIntAttribute::SetBaseValue(int UValue)
{
	this->BaseValue = UValue;
	this->Refresh();
}

void UIntOperator::AddApplied(UBaseIntAttribute* Attr)
{
	if (IsValid(Attr))
	{
		this->Applied.Add(Attr);
	}
}

void UIntOperator::RemoveApplied(UBaseIntAttribute* Attr)
{
	if (IsValid(Attr))
	{
		this->Applied.Remove(Attr);
	}
}

void UIntOperator::Refresh()
{
	for (const auto& App : this->Applied)
	{
		App->Refresh();
	}
}

void UIntOperator::Remove()
{
	TArray<TObjectPtr<UBaseIntAttribute>> Cached = this->Applied;

	for (const auto& App : Cached)
	{
		App->UnOperate(this);
	}
}

void UIntegerPropertySetter::Initialize_Inner_Implementation()
{
	if (this->bInline)
	{
		this->SourceProperty = this->Component->FindRPGPropertyByName(this->SourcePropertyName);
	}
}

void UIntegerPropertySetter::ApplyValue_Implementation()
{
	if (this->bInline)
	{
		IIntegerRPGProperty::Execute_SetIntegerValue(this->Property, this->Value);
	}
	else
	{
		IIntegerRPGProperty::Execute_SetIntegerValue(
			this->Property,
			IIntegerRPGProperty::Execute_GetIntegerValue(this->SourceProperty));
	}
}

void UIntegerPropertyCompare::Initialize_Inner_Implementation()
{
	if (this->bInline)
	{
		this->CompareProperty = this->Component->FindRPGPropertyByName(this->ComparePropertyName);
		check(this->CompareProperty)
	}
}

bool UIntegerPropertyCompare::Compare_Implementation()
{
	if (this->bInline)
	{
		return NumericComparison::Compare(
			this->Comparison,
			IIntegerRPGProperty::Execute_GetIntegerValue(this->Property),
			this->Value);
	}
	else
	{
		return NumericComparison::Compare(
			this->Comparison,
			IIntegerRPGProperty::Execute_GetIntegerValue(this->Property),
			IIntegerRPGProperty::Execute_GetIntegerValue(this->CompareProperty));
	}
}


