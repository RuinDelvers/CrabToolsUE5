#include "StateMachine/Transitions/IsTypeTransition.h"
#include "Kismet/KismetSystemLibrary.h"



bool UIsTypeTransitionDataCondition::Check(UObject* Obj) const
{
	bool ValidType = false;
	bool ValidInterface = true;

	if (IsValid(Obj))
	{
		if (IsValid(this->Class))
		{
			ValidType = Obj->IsA(this->Class.Get());
		}

		
		for (const auto& IFace : this->Interface)
		{
			if (IsValid(IFace))
			{
				if (!Obj->GetClass()->ImplementsInterface(IFace))
				{
					ValidInterface = false;
					break;
				}
			}
		}
	}

	return ValidType && ValidInterface;
}