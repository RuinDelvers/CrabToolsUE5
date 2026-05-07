#include "Widgets/RPGResourceProgressBar.h"
#include "Components/RPGSystem/RPGProperty.h"

void URPGResourceProgressBar::SetProperty(URPGResource* Prop)
{
	if (this->Property)
	{
		this->Property->OnPropertyChanged.RemoveAll(this);
		this->Property->GetMaxRef()->OnPropertyChanged.RemoveAll(this);
		this->Property->GetMinRef()->OnPropertyChanged.RemoveAll(this);
	}

	this->Property = Prop;

	if (this->Property)
	{
		this->Property->OnPropertyChanged.AddDynamic(this, &URPGResourceProgressBar::OnPropertyChanged);
		this->Property->GetMaxRef()->OnPropertyChanged.AddDynamic(this, &URPGResourceProgressBar::OnPropertyChanged);
		this->Property->GetMinRef()->OnPropertyChanged.AddDynamic(this, &URPGResourceProgressBar::OnPropertyChanged);
		
		this->OnPropertyChanged(this->Property);
	}
	else
	{
		this->SetPercent(0);
	}

	
}

void URPGResourceProgressBar::OnPropertyChanged(URPGProperty* Prop)
{
	this->SetPercent(CastChecked<URPGResource>(this->Property)->GetPercent());
}
