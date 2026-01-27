#include "Widgets/RPGResourceProgressBar.h"
#include "Components/RPGSystem/RPGProperty.h"

void URPGResourceProgressBar::SetProperty(URPGResource* Prop)
{
	if (this->Property)
	{
		this->Property->OnPropertyChanged.RemoveAll(this);
	}

	this->Property = Prop;

	if (this->Property)
	{

		this->Property->OnPropertyChanged.AddDynamic(this, &URPGResourceProgressBar::OnPropertyChanged);
		this->OnPropertyChanged(this->Property);
	}
	else
	{
		this->SetPercent(0);
	}

	
}

void URPGResourceProgressBar::OnPropertyChanged(URPGProperty* Prop)
{
	this->SetPercent(CastChecked<URPGResource>(Prop)->GetPercent());
}
