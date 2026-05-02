#include "Ability/Ability.h"

DEFINE_LOG_CATEGORY(LogAbility);

UAbility::UAbility(const FObjectInitializer& ObjInit)
{
	this->AbilityData = CreateDefaultSubobject<UInlineAbilityData>(TEXT("AbilityData"));
}

void UAbility::Initialize(AActor* POwner, UObject* InitData)
{
	this->Owner = POwner;

	this->AbilityData->Initialize(InitData);
	this->Initialize_Inner(InitData);
}

UAbilityData* UAbility::SetAbilityData(TSubclassOf<UAbilityData> DataClass)
{
	this->AbilityData->OnDataChanged.Clear();

	this->AbilityData = NewObject<UAbilityData>(this, DataClass);

	return this->AbilityData;
}

void UAbility::Start(UObject* StartData)
{
	if (!this->bActive && this->bUseable)
	{
		this->bActive = true;
		this->CurrentStartData = StartData;
		this->AbilityData->Start(StartData);
		this->Start_Inner();
		this->OnAbilityStarted.Broadcast(this);		
	}
}

void UAbility::Detach()
{
	this->Finish();
	this->SetUseable(false);
	this->Detach_Inner();
	this->Owner = nullptr;	
}

UAbility* UAbility::GetParent() const
{
	return Cast<UAbility>(this->GetOuter());
}

AActor* UAbility::GetOwner() const
{
	return this->Owner;
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
		this->CurrentStartData = nullptr;
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

UInlineAbilityData::UInlineAbilityData()
: AbilityRange(std::numeric_limits<float>::infinity())
{

}

void UAbilityData::Update()
{
	this->OnDataChanged.Broadcast(this);
}

void UAbilityData::Start(UObject* StartData)
{
	this->CurrentStartData = StartData;
	this->Start_Inner();
}

void UAbilityData::Finish()
{
	this->Finish_Inner();
	this->CurrentStartData = nullptr;
}
