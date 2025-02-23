#include "Utils/UtilsLibrary.h"
#include "Utils/Enums.h"

void UUtilsLibrary::ActivateTimeGatedBool(FTimeGatedBool& input) {
	input.Reset();
}

bool UUtilsLibrary::TimeGatedBoolConvert(const FTimeGatedBool& input) {
	return input.GetValue();
}

float UUtilsLibrary::RotateAngleTo(float Base, float Goal, float Delta)
{
	bool Complete = false;
	float RealDelta = RotateAngleToDelta(Base, Goal, Delta, Complete);

	if (Complete)
	{
		return Goal;
	}
	else
	{
		return Base + RealDelta;
	}
}

float UUtilsLibrary::RotateAngleToDelta(float Base, float Goal, float Delta, bool& Complete)
{
	// Normalize the angles to [-180, 180] and to [0, 360].
	Base = FRotator::NormalizeAxis(Base);
	Goal = FRotator::NormalizeAxis(Goal);
	float DeBase = Base < 0 ? Base + 360 : Base;
	float DeGoal = Goal < 0 ? Goal + 360 : Goal;

	// Find the relative diffs for different ranges.
	float DiffNorm = Goal - Base;
	float DiffUnNorm = DeGoal - DeBase;

	// Whichever is the smallest diff will be the true difference in the angle.
	float Diff = FMath::Abs(DiffNorm) < FMath::Abs(DiffUnNorm) ? DiffNorm : DiffUnNorm;

	if (FMath::Abs(Diff) < FMath::Abs(Delta))
	{
		Complete = true;
		return Diff;
	}
	else
	{
		Complete = false;
		return FMath::Sign(Diff) * Delta;
	}
}

bool UUtilsLibrary::Contains(const FSetGatedBool& Input, UObject* Obj) {
	return Input.ContainsObj(Obj);
}

void UUtilsLibrary::GateObj(FSetGatedBool& Input, UObject* Obj) {
	Input.AddObj(Obj);
}

UObject* UUtilsLibrary::GetOuterAs(UObject* Object, TSubclassOf<UObject> SClass, ESearchResult& Result)
{
	if (!IsValid(Object))
	{
		Result = ESearchResult::NotFound;
		return nullptr;
	}

	auto Outer = Object->GetOuter();

	while (Outer)
	{
		if (Outer->GetClass()->IsChildOf(SClass))
		{
			break;
		}
		else
		{
			Outer = Outer->GetOuter();
		}
	}

	if (Outer)
	{
		Result = ESearchResult::Found;
	}
	else
	{
		Result = ESearchResult::NotFound;
	}

	return Outer;
}

UObject* UUtilsLibrary::GetOwnerAs(UActorComponent* Component, TSubclassOf<AActor> SClass, ESearchResult& Result)
{
	auto Class = SClass.Get();
	auto Owner = Component->GetOwner();
	if (Class && Owner) {
		if (Owner->IsA(Class)) {
			Result = ESearchResult::Found;
			return Owner;
		}
	}

	Result = ESearchResult::Found;
	return nullptr;
}

void UUtilsLibrary::ActivateNAryGate(FNAryGate& Gate, ETriggerBranch& Result)
{
	Result = Gate.Activate();
}

void UUtilsLibrary::DeactivateNAryGate(FNAryGate& Gate, ETriggerBranch& Result)
{
	Result = Gate.Deactivate();
}