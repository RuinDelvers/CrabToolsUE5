#include "StateMachine/Transitions/BooleanTransition.h"

UBooleanTransitionCondition::UBooleanTransitionCondition()
{
	this->Property = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("Property"));
}

bool UBooleanTransitionCondition::Check() const
{
	bool bReadValue = this->Property->GetBool();
	UE_LOG(LogTemp, Warning, TEXT("Checking boolean flag "));

	if (this->bRequiredValue)
	{
		UE_LOG(LogTemp, Warning, TEXT("- Required value is true... "));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("- Required value is false... "));
	}

	if (bReadValue)
	{
		UE_LOG(LogTemp, Warning, TEXT("- Read value is true... "));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("- Required value is false... "));
	}

	return bReadValue == this->bRequiredValue;
}

void UBooleanTransitionCondition::Initialize_Inner_Implementation()
{
	this->Property->Initialize();
}

UBooleanTransitionDataCondition::UBooleanTransitionDataCondition()
{
	this->Property = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("Property"));
}

bool UBooleanTransitionDataCondition::Check(UObject* Data) const
{
	return this->Property->GetBool() == this->bRequiredValue;
}

void UBooleanTransitionDataCondition::Initialize_Inner_Implementation()
{
	this->Property->Initialize();
}