#include "StateMachine/Transitions/BooleanTransition.h"

UBooleanTransitionCondition::UBooleanTransitionCondition()
{
	
}

bool UBooleanTransitionCondition::Check() const
{
	bool bReadValue = this->Property.Get(this);

	return bReadValue == this->bRequiredValue;
}


UBooleanTransitionDataCondition::UBooleanTransitionDataCondition()
{
	
}

bool UBooleanTransitionDataCondition::Check(UObject* Data) const
{
	return this->Property.Get(this) == this->bRequiredValue;
}
