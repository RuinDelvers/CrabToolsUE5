#include "StateMachine/Transitions/BaseTransitions.h"
#include "Utils/UtilsLibrary.h"

TObjectPtr<UTrueTransitionCondition> UTrueTransitionCondition::Singleton = nullptr;
TObjectPtr<UTrueTransitionDataCondition> UTrueTransitionDataCondition::Singleton = nullptr;

UTransitionCondition* UTrueTransitionCondition::GetStaticTransition()
{
	if (!IsValid(Singleton))
	{
		Singleton = NewObject<UTrueTransitionCondition>();
		Singleton->AddToRoot();
	}

	return Singleton;
}

UTransitionDataCondition* UTrueTransitionDataCondition::GetStaticTransition()
{
	if (!IsValid(Singleton))
	{
		Singleton = NewObject<UTrueTransitionDataCondition>();
		Singleton->AddToRoot();
	}

	return Singleton;
}