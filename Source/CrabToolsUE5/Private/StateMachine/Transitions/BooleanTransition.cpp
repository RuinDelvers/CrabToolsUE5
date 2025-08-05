#include "StateMachine/Transitions/BooleanTransition.h"

UBooleanTransitionCondition::UBooleanTransitionCondition()
{
	this->Property = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("Property"));
}

bool UBooleanTransitionCondition::Check() const
{
	return this->Property->GetBool() == this->bRequiredValue;
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