#include "StateMachine/Transitions/FunctionCallTransition.h"
#include "Utils/UtilsLibrary.h"

void UFunctionTransitionCondition::Initialize_Inner_Implementation()
{
	this->Callback.BindUFunction(this->GetMachine(), this->FunctionName);
}

bool UFunctionTransitionCondition::Check() const
{
	if (this->Callback.IsBound())
	{
		return this->Callback.Execute();
	}
	else
	{
		return false;
	}
}


#if WITH_EDITOR
TArray<FString> UFunctionTransitionCondition::GetFunctionOptions() const
{
	if (auto SMLike = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		return SMLike->GetConditionOptions();
	}
	else
	{
		return {};
	}
}
#endif // WITH_EDITOR

void UFunctionTransitionDataCondition::Initialize_Inner_Implementation()
{
	this->Callback.BindUFunction(this->GetMachine(), this->FunctionName);
}

bool UFunctionTransitionDataCondition::Check(UObject* Data) const
{
	if (this->Callback.IsBound())
	{
		return this->Callback.Execute(Data);
	}
	else
	{
		return false;
	}
}

#if WITH_EDITOR
TArray<FString> UFunctionTransitionDataCondition::GetFunctionOptions() const
{
	if (auto SMLike = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		return SMLike->GetDataConditionOptions();
	}
	else
	{
		return {};
	}
}
#endif // WITH_EDITOR