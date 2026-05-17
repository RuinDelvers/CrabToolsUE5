#include "Actors/Targeting/InlineTargeting.h"

UInlineTargeting::UInlineTargeting(const FObjectInitializer& Init)
{
}

void UInlineTargeting::GetTargetData_Implementation(TArray<FTargetingData>& OutData) const
{
	OutData.Append(this->Data);
}

void UInlineTargeting::AddTarget_Implementation(const FTargetingData& TargetData)
{
	this->Data.Add(TargetData);
}
