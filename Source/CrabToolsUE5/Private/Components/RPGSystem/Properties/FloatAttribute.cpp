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

void UBaseFloatAttribute::Refresh()
{
	auto Value = this->GetBaseValue();

	for (auto& Op : this->Operators)
	{
		Value = Op->Operate(Value);
	}

	this->CompValue = Value;

	this->OnAttributeChanged.Broadcast(this);
}


void UFloatAttribute::SetBaseValue(float UValue)
{
	this->BaseValue = UValue;
	this->Refresh();
}