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
		this->Refresh();
	}
}

void UBaseIntAttribute::UnOperate(UIntOperator* Op)
{
	if (IsValid(Op))
	{
		this->Operators.Remove(Op);
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