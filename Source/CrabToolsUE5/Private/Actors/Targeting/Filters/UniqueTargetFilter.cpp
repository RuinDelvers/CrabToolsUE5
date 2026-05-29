#include "Actors/Targeting/Filters/UniqueTargetFilter.h"

#define LOCTEXT_NAMESPACE "UniqueTargetFilterComponent"

UUniqueTargetFilterComponent::UUniqueTargetFilterComponent()
{
	this->ErrorText = LOCTEXT("NonUniqueTargetError", "Cannot Target Twice.");
}

bool UUniqueTargetFilterComponent::Filter_Implementation(const FTargetingData& Data, FText& FailureReason)
{
	bool Unique = true;

	ITargetingControllerInterface::Execute_GetTargetData(this->GetOwner(), this->DataCache);

	for (const auto& CurrentData : this->DataCache)
	{
		if (Data.TargetActor == CurrentData.TargetActor)
		{
			FailureReason = this->ErrorText;
			Unique = false;
		}
	}

	this->DataCache.Empty();

	return Unique;
}

#undef LOCTEXT_NAMESPACE