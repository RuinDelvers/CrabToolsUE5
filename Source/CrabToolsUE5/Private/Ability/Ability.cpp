#include "Ability/Ability.h"

DEFINE_LOG_CATEGORY(LogAbility);

void UAbility::Initialize(AActor* POwner)
{
	if (POwner)
	{
		if (!this->Owner)
		{
			this->Owner = POwner;
		}
	}
	else
	{
		if (this->Owner)
		{
			this->Owner = this->GetOwner();
		}
	}
	this->Initialize_Inner();
}

void UAbility::Start()
{
	if (!this->bActive && this->bUseable)
	{
		this->bActive = true;
		this->Start_Inner();
		this->OnAbilityStarted.Broadcast(this);		
	}
}

UAbility* UAbility::GetParent() const
{
	return Cast<UAbility>(this->GetOuter());
}

AActor* UAbility::GetOwner() const
{
	if (this->Owner)
	{
		return this->Owner;
	}
	else if (auto Parent = this->GetParent())
	{
		return Parent->GetOwner();
	}
	else
	{
		return nullptr;
	}
}

UAbility* UAbility::GetParentAs(TSubclassOf<UAbility> ParentClass) const
{
	auto Outer = this->GetOuter();

	if (Outer->IsA(ParentClass))
	{
		return Cast<UAbility>(Outer);
	}
	return nullptr;
}

void UAbility::Perform()
{
	if (this->bActive)
	{
		this->Perform_Inner();
		this->OnAbilityPerformed.Broadcast(this);
	}
}

void UAbility::SetUseable(bool bNewUseable)
{
	this->bUseable = bNewUseable;
	this->OnAbilityUseabilityChanged.Broadcast(this, bNewUseable);
}

void UAbility::Delete()
{
	this->Delete_Inner();
	this->OnAbilityDeleted.Broadcast(this);
}

void UAbility::Tick(float DeltaTime)
{
	if (this->bActive)
	{
		this->Tick_Inner(DeltaTime);
	}
}

void UAbility::Finish()
{
	if (this->bActive)
	{
		this->bActive = false;
		this->Finish_Inner();
		this->OnAbilityFinished.Broadcast(this);
	}
}

void UAbility::Cancel()
{
	this->Cancel_Inner();
	this->OnAbilityCanceled.Broadcast(this);
}
