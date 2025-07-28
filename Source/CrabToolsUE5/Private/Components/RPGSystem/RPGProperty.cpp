#include "Components/RPGSystem/RPGProperty.h"

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