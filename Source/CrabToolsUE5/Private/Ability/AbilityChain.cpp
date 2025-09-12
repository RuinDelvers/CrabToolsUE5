#include "Ability/AbilityChain.h"

void UAbilityChain::Initialize_Inner_Implementation()
{
	this->ActiveIndex = 0;
	
	for (auto& Abi : this->AbilityChain)
	{
		Abi->Initialize(this->GetOwner());
	}
}

void UAbilityChain::Start_Inner_Implementation()
{
	this->ActiveIndex = 0;

	if (this->AbilityChain.Num() > 0)
	{
		auto& Abi = this->AbilityChain[0];
		Abi->OnAbilityFinished.AddDynamic(this, &UAbilityChain::HandleFinish);
		Abi->Start();
	}
	else
	{
		this->Finish();
	}
}

void UAbilityChain::Perform_Inner_Implementation()
{
	this->AbilityChain[this->ActiveIndex]->Perform();
}

void UAbilityChain::Tick_Inner_Implementation(float DeltaTime)
{
	this->AbilityChain[this->ActiveIndex]->Tick(DeltaTime);
}

void UAbilityChain::Finish_Inner_Implementation()
{
	if (this->ActiveIndex < this->AbilityChain.Num())
	{
		this->AbilityChain[this->ActiveIndex]->OnAbilityFinished.RemoveAll(this);
	}
}

bool UAbilityChain::RequiresTick_Implementation() const
{
	for (auto& Abi : this->AbilityChain)
	{
		if (Abi->RequiresTick())
		{
			return true;
		}
	}

	return false;
}

void UAbilityChain::HandleFinish(UAbility* _Abi)
{
	auto& AbiOld = this->AbilityChain[this->ActiveIndex];
	AbiOld->OnAbilityFinished.RemoveAll(this);

	this->ActiveIndex += 1;

	if (this->ActiveIndex < this->AbilityChain.Num())
	{
		auto& AbiNew = this->AbilityChain[this->ActiveIndex];
		AbiNew->OnAbilityFinished.AddDynamic(this, &UAbilityChain::HandleFinish);
		AbiNew->Start();
	}
	else
	{
		this->Finish();
	}
}

UAbility* UAbilityChain::GetChild(TSubclassOf<UAbility> Type, int Index, ESearchResult& Result) const
{
	if (Index < this->AbilityChain.Num())
	{
		auto& Child = this->AbilityChain[Index];
		if (Child && Child->IsA(Type))
		{
			Result = ESearchResult::Found;
			return Child;
		}
	}

	Result = ESearchResult::NotFound;
	return nullptr;
}