#include "Components/RPGSystem/RPGProperty.h"
#include "Components/RPGSystem/RPGComponent.h"
#include "Utils/UtilsLibrary.h"

void URPGProperty::Initialize(URPGComponent* InitOwner)
{
	this->Owner = InitOwner;
	this->Initialize_Inner();
}

void URPGProperty::Initialize_Inner_Implementation()
{

}

FText URPGProperty::GetDisplayText_Implementation() const
{
	return FText::FromString("Undefined RPG Property Text");
}

void URPGOperation::Initialize(URPGComponent* InitComponent)
{
	#if WITH_EDITOR
		check(InitComponent);
		check(InitComponent->IsA(this->PropertySource.LoadSynchronous()));
	#endif

	this->Component = Component;
	this->Property = InitComponent->FindRPGPropertyByName(this->PropertyName);

	check(this->Property);

	this->Initialize_Inner();
}

void URPGOperation::SetExternalControl(
	bool bNewExternalControl,
	TSoftClassPtr<URPGComponent> Comp,
	FName ExternalPropName)
{
	this->bExternalControlProperty = bNewExternalControl;
	this->PropertySource = Comp;
}

TArray<FString> URPGOperation::GetPropertyNames() const
{
	TArray<FString> Names;

	
	if (auto Source = this->PropertySource.LoadSynchronous())
	{
		auto Type = this->GetPropertySearchType();
		if (Type)
		{
			if (Type->IsChildOf<UInterface>())
			{
				Names.Append(Source->GetDefaultObject<URPGComponent>()->GetRPGPropertyNamesFromInterface(
					Type,
					false));
			}
			else
			{
				Names.Append(Source->GetDefaultObject<URPGComponent>()->GetRPGPropertyNames(
					Type,
					false));
			}
		}
		
	}

	Names.Sort([](const FString& A, const FString& B) { return A < B; });
	return Names;
}

TArray<FString> URPGResource::GetAttributeOptions() const
{
	if (auto Outer = UtilsFunctions::GetOuterAs<URPGComponent>(this))
	{
		return Outer->GetRPGPropertyNames(this->GetBoundsType());
	}

	return {};
}
