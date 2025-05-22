#include "Components/RPGSystem/RPGHelpers.h"

template <class ValueType, class CallbackType> inline void ObserveValue(ValueType& input, const CallbackType& Callback, bool bRemove)
{
	if (bRemove)
	{
		input.ValueChangedEvent.Remove(Callback);
	}
	else
	{
		if (!input.ValueChangedEvent.Contains(Callback))
		{
			input.ValueChangedEvent.Add(Callback);
		}
	}
}

#pragma region Integer Attributes and Resources
void URPGHelpers::ObserveIntAttribute(FIntAttribute& input, const FIntAttributeCallback& Callback, bool bRemove) {
	ObserveValue(input, Callback, bRemove);
}

void URPGHelpers::ObserveIntResource(FIntResource& input, const FIntAttributeCallback& Callback, bool bRemove) {
	ObserveValue(input, Callback, bRemove);
}

void URPGHelpers::SetIntResource(UPARAM(ref) FIntResource& input, int Value) {
	input.SetValue(Value);
}

void URPGHelpers::SetIntAttribute(UPARAM(ref) FIntAttribute& input, int Value)
{
	input.SetValue(Value);
}

int URPGHelpers::UnwrapIntAttribute(const FIntAttribute& input) {
	return input.GetValue();
}

int URPGHelpers::UnwrapIntResource(const FIntResource& input) {
	return input.GetValue();
}


int URPGHelpers::MaxIntRescValue(const FIntResource& Input) {
	return Input.GetMax();
}


int URPGHelpers::MinIntRescValue(const FIntResource& Input) {
	return Input.GetMin();
}

float URPGHelpers::IntRescPercent(const FIntResource& Input) {
	return Input.GetPercent();
}

void URPGHelpers::AddIntResource(FIntResource& input, int Amount)
{
	input.SetValue(input.Value + Amount);
}

void URPGHelpers::SubtractIntResource(FIntResource& input, int Amount)
{
	input.SetValue(input.Value - Amount);
}

#pragma endregion


#pragma region Float Attributes and Resources

void URPGHelpers::ObserveFloatAttribute(FFloatAttribute& input, const FFloatAttributeCallback& Callback, bool bRemove) {
	ObserveValue(input, Callback, bRemove);
}

void URPGHelpers::ObserveFloatResource(FFloatResource& input, const FFloatAttributeCallback& Callback, bool bRemove) {
	ObserveValue(input, Callback, bRemove);
}

void URPGHelpers::SetFloatResource(UPARAM(ref) FFloatResource& input, float Value) {
	input.SetValue(Value);
}

void URPGHelpers::SetFloatAttribute(UPARAM(ref) FFloatAttribute& input, float Value)
{
	input.SetValue(Value);
}

float URPGHelpers::UnwrapFloatAttribute(const FFloatAttribute& input) {
	return input.GetValue();
}

float URPGHelpers::UnwrapFloatResource(const FFloatResource& input) {
	return input.GetValue();
}

float URPGHelpers::MaxFloatRescValue(const FFloatResource& Input) {
	return Input.GetMax();
}

float URPGHelpers::MinFloatRescValue(const FFloatResource& Input) {
	return Input.GetMin();
}

float URPGHelpers::FloatRescPercent(const FFloatResource& Input) {
	return Input.GetPercent();
}

void URPGHelpers::AddFloatResource(FFloatResource& input, float Amount)
{
	input.SetValue(input.Value + Amount);
}

void URPGHelpers::SubtractFloatResource(FFloatResource& input, float Amount)
{
	input.SetValue(input.Value - Amount);
}

#pragma endregion