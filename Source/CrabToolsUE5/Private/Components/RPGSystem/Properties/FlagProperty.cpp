#include "Components/RPGSystem/Properties/FlagProperty.h"

void UFlagProperty::SetFlag(bool bNewFlag)
{
	this->bFlag = bNewFlag;
	this->OnPropertyChanged.Broadcast(this);
}

bool UFlagProperty::GetFlag() const
{
	return this->bFlag;
}
