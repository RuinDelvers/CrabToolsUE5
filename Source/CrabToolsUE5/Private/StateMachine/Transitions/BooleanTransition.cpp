#include "StateMachine/Transitions/BooleanTransition.h"

UBooleanTransitionCondition::UBooleanTransitionCondition()
{
	this->Property = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("Property"));
}

bool UBooleanTransitionCondition::Check() const
{
	return this->Property->GetBool() == this->bRequiredValue;
}

UBooleanTransitionDataCondition::UBooleanTransitionDataCondition()
{
	this->Property = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("Property"));
}

bool UBooleanTransitionDataCondition::Check(UObject* Data) const
{
	return this->Property->GetBool() == this->bRequiredValue;
}
