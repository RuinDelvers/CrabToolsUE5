#include "StateMachine/Transitions/BooleanTransition.h"
#include "Logging/MessageLog.h"

UBooleanTransitionCondition::UBooleanTransitionCondition()
{
	
}

bool UBooleanTransitionCondition::Check() const
{
	#if WITH_EDITOR
		if (!this->Property.IsValid())
		{
			FMessageLog Log("BooleanTransition::Debug");
			FString Message = FString::Printf(
				TEXT("Invalid property in boolean transition: %s"),
				*this->GetMachine()->GetFullMachinePath());
			Log.Error(FText::FromString(Message));
		}
	#endif
	return this->Property.Get(this) == this->bRequiredValue;
}


UBooleanTransitionDataCondition::UBooleanTransitionDataCondition()
{
	
}

bool UBooleanTransitionDataCondition::Check(UObject* Data) const
{
	#if WITH_EDITOR
		if (!this->Property.IsValid())
		{
			FMessageLog Log("BooleanTransition::Debug");
			FString Message = FString::Printf(
				TEXT("Invalid property in boolean transition: %s"),
				*this->GetMachine()->GetFullMachinePath());
			Log.Error(FText::FromString(Message));
		}
	#endif
	return this->Property.Get(this) == this->bRequiredValue;
}
