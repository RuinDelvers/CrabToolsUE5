#include "Ability/Animation/AnimAbility.h"
#include "GameFramework/Character.h"
#include "Utils/UtilsLibrary.h"

UAnimAbility::UAnimAbility()
{
	#if WITH_EDITORONLY_DATA
		this->ActorClass = ACharacter::StaticClass();
		// Default name for character meshes.
	#endif

	this->ComponentName = "CharacterMesh0";
}

void UAnimAbility::Initialize_Inner_Implementation()
{
	bool bFoundComp = false;

	for (auto& Components : this->GetOwner()->GetComponents())
	{
		if (auto SkComp = Cast<USkeletalMeshComponent>(Components))
		{
			if (Components->GetFName() == this->ComponentName)
			{
				this->Component = SkComp;
				bFoundComp = true;
				break;
			}
		}
	}

	if (!bFoundComp)
	{
		UE_LOG(LogAbility, Error, TEXT("Failed to find SkeletalMeshComponent %s from object %s"),
			*this->ComponentName.ToString(),
			*this->GetOwner()->GetName());
	}
}

void UAnimAbility::Start_Inner_Implementation()
{
	if (this->Component)
	{
		if (auto AnimInst = this->Component->GetAnimInstance())
		{
			AnimInst->AddExternalNotifyHandler(
				this,
				GET_FUNCTION_NAME_CHECKED(UAnimAbility, AnimNotify_AbilityFinish));

			AnimInst->AddExternalNotifyHandler(
				this,
				GET_FUNCTION_NAME_CHECKED(UAnimAbility, AnimNotify_AbilityPerform));

			for (const auto& FnName : this->CustomPhases)
			{
				AnimInst->AddExternalNotifyHandler(this, FnName);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AnimAbility(%s) - Could not find anim instance."), *this->GetName());
			this->Finish();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AnimAbility(%s) - Could not find skeletal mesh component."), *this->GetName());
		this->Finish();
	}
}

void UAnimAbility::Finish_Inner_Implementation()
{
	if (this->Component)
	{
		if (auto AnimInst = this->Component->GetAnimInstance())
		{
			AnimInst->RemoveExternalNotifyHandler(
				this,
				GET_FUNCTION_NAME_CHECKED(UAnimAbility, AnimNotify_AbilityFinish));

			AnimInst->RemoveExternalNotifyHandler(
				this,
				GET_FUNCTION_NAME_CHECKED(UAnimAbility, AnimNotify_AbilityPerform));

			for (const auto& FnName : this->CustomPhases)
			{
				AnimInst->RemoveExternalNotifyHandler(this, FnName);
			}
		}
	}
}

void UAnimAbility::AnimNotify_AbilityFinish()
{
	this->Finish();
}

void UAnimAbility::AnimNotify_AbilityPerform()
{
	this->Perform();
}

#if WITH_EDITOR
TArray<FString> UAnimAbility::GetComponentOptions() const
{
	TArray<FString> Names;

	if (this->ActorClass)
	{
		for (auto& SkComp : UtilsFunctions::GetComponentsByClass<USkeletalMeshComponent>(this->ActorClass))
		{
			Names.Add(SkComp->GetName());
		}

		Names.Sort([&](const FString& A, const FString& B) { return A < B; });
	}

	return Names;
}


TArray<FString> UAnimAbility::GetCustomPhases() const
{
	TArray<FString> Names;
	const auto Compare = this->FindFunction(GET_FUNCTION_NAME_CHECKED(UAnimAbility, AnimNotify_AbilityPerform));

	for (TFieldIterator<UFunction> FIT(this->GetClass(), EFieldIterationFlags::IncludeSuper); FIT; ++FIT)
	{
		FString FnName = FIT->GetName();

		bool c1 = FnName.StartsWith("AnimNotify_");
		bool c2 = FnName != GET_FUNCTION_NAME_CHECKED(UAnimAbility, AnimNotify_AbilityFinish);
		bool c3 = FnName != GET_FUNCTION_NAME_CHECKED(UAnimAbility, AnimNotify_AbilityPerform);
		bool c4 = FIT->IsSignatureCompatibleWith(Compare);

		if (c1 && c2 && c3 && c4)
		{
			Names.Add(FnName);
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

void UAnimAbility::FilterPhases()
{
	TSet<FName> Keep;

	for (const auto& FnName : this->CustomPhases)
	{
		if (this->FindFunction(FnName))
		{
			Keep.Add(FnName);
		}
	}

	this->CustomPhases = this->CustomPhases.Intersect(Keep);
}

void UAnimAbility::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	this->FilterPhases();
}

void UAnimAbility::PostLoad()
{
	Super::PostLoad();
	this->FilterPhases();
}

void UAnimAbility::PostLinkerChange()
{
	Super::PostLinkerChange();
	this->FilterPhases();
}

#endif //WITH_EDITOR