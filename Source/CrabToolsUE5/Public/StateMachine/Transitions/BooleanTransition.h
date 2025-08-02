#pragma once

#include "StateMachine/Transitions/BaseTransitions.h"
#include "Properties/GenericPropertyBinding.h"
#include "BooleanTransition.generated.h"

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, CollapseCategories, DisplayName = "BoolFlag")
class CRABTOOLSUE5_API UBooleanTransitionCondition : public UTransitionCondition
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Properties", meta = (ShowInnerProperties))
	TObjectPtr<UGenericPropertyBinding> Property;

	UPROPERTY(EditDefaultsOnly, Category = "Properties", meta = (AllowPrivateAccess))
	bool bRequiredValue = true;

public:

	UBooleanTransitionCondition();

	virtual bool Check() const override;
};

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced, CollapseCategories, DisplayName = "BoolFlag")
class CRABTOOLSUE5_API UBooleanTransitionDataCondition : public UTransitionDataCondition
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Properties", meta = (ShowInnerProperties))
	TObjectPtr<UGenericPropertyBinding> Property;

	UPROPERTY(EditDefaultsOnly, Category = "Properties", meta = (AllowPrivateAccess))
	bool bRequiredValue = true;

public:

	UBooleanTransitionDataCondition();
	virtual bool Check(UObject* Data) const override;
};