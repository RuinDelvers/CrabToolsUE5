#include "Widgets/RPGPropertyTextBlock.h"
#include "Components/RPGSystem/RPGProperty.h"

void URPGPropertyTextBlock::SetProperty(URPGProperty* Prop)
{
	if (this->Property)
	{
		Property->OnPropertyChanged.RemoveAll(this);
	}

	this->Property = Prop;

	if (this->Property)
	{
		this->Property->OnPropertyChanged.AddDynamic(this, &URPGPropertyTextBlock::OnPropertyChanged);
		this->OnPropertyChanged(Prop);
	}
}

void URPGPropertyTextBlock::OnPropertyChanged(URPGProperty* Prop)
{
	this->SetText(this->Property->GetDisplayText());
}