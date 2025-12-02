#include "Utils/UtilsLibrary.h"
#include "Utils/Enums.h"
#include "Engine/InputDelegateBinding.h"

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

TSubclassOf<UObject> UUtilsLibrary::GenerateDynamicClass(TSubclassOf<UObject> Parent, UObject* Owner)
{
	if (!Parent || !Owner)
	{
		return nullptr;
	}

	FName GenClassName = FName(FString::Printf(TEXT("gen_%s_from_%s"), *Parent->GetName(), *Owner->GetName()));

	UClass* GenClass = NewObject<UClass>(Owner->GetPackage(), GenClassName, RF_Public);
	GenClass->PurgeClass(false);

	GenClass->PropertyLink = Parent->PropertyLink;
	GenClass->CppClassStaticFunctions.SetAddReferencedObjects(Parent->CppClassStaticFunctions.GetAddReferencedObjects());
	GenClass->SetSuperStruct(Parent);
	GenClass->StaticLink(true);
	GenClass->AssembleReferenceTokenStream();
	GenClass->Bind();

	return GenClass;
}

void UUtilsLibrary::BindObjectToEnhancedInput(UObject* Obj, APawn* Pawn)
{
	if (IsValid(Obj) && IsValid(Pawn))
	{
		UInputDelegateBinding::BindInputDelegates(Obj->GetClass(), Pawn->InputComponent, Obj);
	}
}

void UUtilsLibrary::UnbindObjectToEnhancedInput(UObject* Obj, APawn* Pawn)
{
	if (IsValid(Obj) && IsValid(Pawn))
	{
		Pawn->InputComponent->ClearBindingsForObject(Obj);
	}
}

bool UUtilsLibrary::IsWithinCone(FVector Location, FVector Base, FVector Endpoint, float Radius, float Exponent)
{
	const auto Dir = (Base - Endpoint);
	const auto Height = Dir.Length();
	const auto DirNormed = Dir.GetUnsafeNormal();
	const float Dist = (Location - Endpoint).Dot(DirNormed);

	if (Dist >= 0 && Dist <= Height)
	{
		const float TestRadius = FMath::Pow((Dist / Height), Exponent) * Radius;
		const auto OrthDist = ((Location - Endpoint) - Dist * DirNormed).Length();

		return OrthDist < TestRadius;
	}
	else
	{
		return false;
	}
}