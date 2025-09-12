#include "Actors/Targeting/BaseTargetingActor.h"
#include "Ability/Ability.h"

ABaseTargetingActor::ABaseTargetingActor()
{
	
}

void ABaseTargetingActor::BeginPlay()
{
	Super::BeginPlay();
	ITargetingControllerInterface::Execute_Initialize(this);
}

void ABaseTargetingActor::Initialize_Implementation()
{
	bool bDoDefaultAttachment = true;

	if (this->bInitUsingAbility)
	{
		if (auto& Ability = this->AbilityOwner)
		{
			if (auto AbiData = Ability->GetData())
			{
				bDoDefaultAttachment = false;
				this->InitFromAbility(AbiData);

				if (AbiData->IsDynamic())
				{
					AbiData->OnDataChanged.AddDynamic(this, &ABaseTargetingActor::InitFromAbility);
				}
			}
		}
	}

	if (bDoDefaultAttachment)
	{
		this->AttachToActor(this->UsingActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

void ABaseTargetingActor::InitFromAbility(UAbilityData* AbiData)
{
	this->MaxTargetCount = AbiData->TargetCount();

	if (auto Comp = AbiData->TargetAttachComponent())
	{
		this->AttachToComponent(
			AbiData->TargetAttachComponent(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			AbiData->TargetAttachPoint());
	}
}

void ABaseTargetingActor::AddListener_Implementation(const FConfirmTargetsSingle& Callback)
{
	this->OnConfirmTargets.Add(Callback);
}

void ABaseTargetingActor::AddDestroyedListener_Implementation(const FTargetingUpdated& Callback)
{
	this->OnDestroyed.Add(Callback);
}


void ABaseTargetingActor::AddTarget_Implementation(const FTargetingData& TargetData)
{
	this->Data.Add(TargetData);
	this->PerformTargetCountCheck();
}

void ABaseTargetingActor::AddDisabledListener_Implementation(const FTargetingUpdated& Callback)
{
	this->OnEnabledUpdated.Add(Callback);
}

void ABaseTargetingActor::AddValidationListener_Implementation(const FValidateTargetingSingle& Callback)
{
	this->OnValidateTargeting.Add(Callback);
}

void ABaseTargetingActor::SetEnabled_Implementation(bool bNewEnabled)
{
	this->bEnabled = bEnabled;
	this->OnEnabledUpdated.Broadcast(this);
}

void ABaseTargetingActor::Confirm_Implementation()
{
	TScriptInterface<ITargetingControllerInterface> Targeter(this);
	this->OnConfirmTargets.Broadcast(Targeter);
}

void ABaseTargetingActor::GetTargetData_Implementation(TArray<FTargetingData>& OutData) const
{
	OutData.Append(this->Data);
}

void ABaseTargetingActor::PopTarget_Implementation()
{
	this->Data.Pop();
}

const TArray<UTargetFilterComponent*>& ABaseTargetingActor::GetFilters() const
{
	if (!this->bFoundFilters)
	{
		for (const auto& Comps : this->GetComponents())
		{
			if (auto Filter = Cast<UTargetFilterComponent>(Comps))
			{
				this->Filters.Add(Filter);
			}
		}

		this->Filters.Sort([](
			const TObjectPtr<UTargetFilterComponent>& A, const TObjectPtr<UTargetFilterComponent>& B)
			{
				return A->GetPriority() > B->GetPriority();
			});

		this->bFoundFilters = true;
	}

	return this->Filters;
}

bool ABaseTargetingActor::ApplyFilters(const FTargetingData& InData, FText& FailureReason) const
{
	for (const auto Filter : this->GetFilters())
	{
		if (!Filter->Filter(InData, FailureReason))
		{
			return false;
		}
	}

	return true;
}

void ABaseTargetingActor::PerformTargetCountCheck()
{
	if (this->MaxTargetCount > 0)
	{
		if (this->Data.Num() >= this->MaxTargetCount)
		{
			ITargetingControllerInterface::Execute_Confirm(this);
		}
	}
}