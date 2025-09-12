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

	this->OnAttributeChanged.Broadcast(this);
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