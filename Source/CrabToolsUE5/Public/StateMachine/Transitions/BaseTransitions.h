#pragma once

#include "StateMachine/StateMachine.h"
#include "BaseTransitions.generated.h"

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, DisplayName = "True")
class CRABTOOLSUE5_API UTrueTransitionCondition : public UTransitionCondition
{

	static TObjectPtr<UTrueTransitionCondition> Singleton;

	GENERATED_BODY()

public:

	static UTransitionCondition* GetStaticTransition();
	virtual bool Check() const override { return true; }
};


/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, DisplayName = "False")
class CRABTOOLSUE5_API UFalseTransitionCondition : public UTransitionCondition
{
	GENERATED_BODY()

public:

	virtual bool Check() const override { return false; }
};

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, DisplayName = "True")
class CRABTOOLSUE5_API UTrueTransitionDataCondition : public UTransitionDataCondition
{
	static TObjectPtr<UTrueTransitionDataCondition> Singleton;

	GENERATED_BODY()

public:

	static UTransitionDataCondition* GetStaticTransition();
	virtual bool Check(UObject* Obj) const override { return true; }
};


/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, DisplayName = "False")
class CRABTOOLSUE5_API UFalseTransitionDataCondition : public UTransitionDataCondition
{
	GENERATED_BODY()

public:

	virtual bool Check(UObject* Obj) const override { return false; }
};

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, DisplayName = "IsValid")
class CRABTOOLSUE5_API UIsValidTransitionDataCondition : public UTransitionDataCondition
{
	GENERATED_BODY()

public:

	virtual bool Check(UObject* Obj) const override { return IsValid(Obj); }
};

