#include "Actors/Targeting/Filters/DistanceTargetFilter.h"
#include "Ability/Ability.h"

#define LOCTEXT_NAMESPACE "DistanceTargetFilter"

UDistanceTargetFilterComponent::UDistanceTargetFilterComponent()
	: ErrorMessage(LOCTEXT("TooFarResult", "Target is too far."))
{

}

void UDistanceTargetFilterComponent::BeginPlay()
{
	Super::BeginPlay();

	if (this->bUseAbilityDistance)
	{
		if (this->GetOwner()->Implements<UHasAbilityInterface>())
		{
			if (auto Ability = IHasAbilityInterface::Execute_GetAbility(this->GetOwner()))
			{
				if (auto Data = Ability->GetData())
				{
					this->InitFromAbility(Data);

					if (Data->IsDynamic())
					{
						Data->OnDataChanged.AddDynamic(this, &UDistanceTargetFilterComponent::InitFromAbility);
					}
				}
				else
				{
					this->MissingAbilityError();
				}
			}
			else
			{
				this->MissingAbilityError();
			}
		}
		else
		{
			this->MissingAbilityError();
		}
	}
}

void UDistanceTargetFilterComponent::InitFromAbility(UAbilityData* Data)
{
	this->Range = Data->Range();
}

FVector UDistanceTargetFilterComponent::GetTargetPoint(const FTargetingData& Data) const
{
	switch (this->TargetPointSource)
	{
		case EDistanceTargetFilterLocationSource::TARGET_ACTOR: return Data.TargetActor->GetActorLocation();
		case EDistanceTargetFilterLocationSource::POINT: return Data.TargetLocation;
		default: return FVector::ZeroVector;
	}
}

FVector UDistanceTargetFilterComponent::GetSourccePoint() const
{
	switch (this->SourcePointSource)
	{
		case EDistanceSourceFilterLocationSource::TARGETING: return this->GetOwner()->GetActorLocation();
		case EDistanceSourceFilterLocationSource::USING_ACTOR:
			return ITargetingControllerInterface::Execute_GetUsingActor(this->GetOwner())->GetActorLocation();
		default: return FVector::ZeroVector;
	}
}

bool UDistanceTargetFilterComponent::Filter_Implementation(const FTargetingData& Data, FText& FailureReason)
{
	
	float Distance = FVector::DistSquared(this->GetTargetPoint(Data), this->GetSourccePoint());
	bool Result = Distance < this->Range * this->Range;

	if (!Result)
	{
		FailureReason = this->ErrorMessage;
	}

	return Result;
}


void UDistanceTargetFilterComponent::MissingAbilityError() const
{
	auto Message = LOCTEXT(
		"InconsistentSettingsError",
		"Distance filter is set to use ability distance, but targeting doesn't have one. ({Owner})");
	FMessageLog Log("Ability");
	Log.Error(FText::FormatNamed(Message,
		TEXT("Owner"), FText::FromString(this->GetOwner()->GetName())));
}

#undef LOCTEXT_NAMESPACE