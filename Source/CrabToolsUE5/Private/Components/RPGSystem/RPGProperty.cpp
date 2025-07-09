#include "Components/RPGSystem/RPGProperty.h"

void URPGProperty::Initialize(URPGComponent* InitOwner)
{
	this->Owner = InitOwner;
	this->Initialize_Inner();
}

void URPGProperty::Initialize_Inner_Implementation()
{

}