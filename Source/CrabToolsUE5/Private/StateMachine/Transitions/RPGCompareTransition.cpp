#include "StateMachine/Transitions/RPGCompareTransition.h"
#include "Components/RPGSystem/RPGComponent.h"
#include "Components/RPGSystem/RPGProperty.h"

void URPGCompareTransitionCondition::Initialize_Inner_Implementation()
{
	this->Compare->Initialize(
		CastChecked<URPGComponent>(this->GetActorOwner()->FindComponentByClass(this->RPGClass.LoadSynchronous())));
}

bool URPGCompareTransitionCondition::Check() const
{
	if (this->Compare)
	{
		return this->Compare->Compare();
	}
	else
	{
		return false;
	}
}



#if WITH_EDITOR
void URPGCompareTransitionCondition::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	if (Event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(URPGCompareTransitionCondition, PropertyName))
	{
		this->UpdateCompareObject();
	}
}

TArray<FString> URPGCompareTransitionCondition::GetPropertyOptions() const
{
	TArray<FString> Names;

	if (IsValid(this->RPGClass.LoadSynchronous()))
	{
		Names.Append(this->RPGClass->GetDefaultObject<URPGComponent>()->GetRPGPropertyNames(
			URPGProperty::StaticClass(),
			false));
	}

	Names.Sort([](const FString& A, const FString& B) { return A < B; });
	return Names;
}

void URPGCompareTransitionCondition::UpdateCompareObject()
{
	if (this->RPGClass.LoadSynchronous())
	{
		if (auto Prop = this->RPGClass->GetDefaultObject<URPGComponent>()->FindDefaultRPGPropertyByName(this->PropertyName))
		{
			UE_LOG(LogTemp, Warning, TEXT("Found property from name %s"), *this->PropertyName.ToString());
			if (auto SetterClass = Prop->GetCompare())
			{
				UE_LOG(LogTemp, Warning, TEXT("- Found property Compare from name %s"), *this->PropertyName.ToString());
				if (!(this->Compare && this->Compare->GetClass() == SetterClass))
				{
					UE_LOG(LogTemp, Warning, TEXT("- - Setting compare"));
					this->Compare = NewObject<URPGCompare>(this, SetterClass);
					this->Compare->SetExternalControl(true, this->RPGClass, this->PropertyName);
				}
			}
			else
			{
				this->Compare = nullptr;
			}
		}
	}
	else
	{
		this->PropertyName = NAME_None;
		this->Compare = nullptr;
	}

	if (this->Compare)
	{
		this->Compare->SetPropertyName(this->PropertyName);
	}
}
#endif