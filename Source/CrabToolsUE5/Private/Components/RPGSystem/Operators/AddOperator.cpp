#include "Components/RPGSystem/Operators/AddOperator.h"

int UIntAddOperator::Operate_Implementation(int Value) {
	return Value + this->Additive;
}

void UIntAddOperator::SetAdditive(int NewAdditive)
{
	this->Additive = NewAdditive;
	this->Refresh();
}