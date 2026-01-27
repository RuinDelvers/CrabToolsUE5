#include "Widgets/RPGResourceProgressBar.h"
#include "Components/RPGSystem/RPGProperty.h"

void URPGResourceProgressBar::SetProperty(URPGResource* Prop)
{
	if (this->Property)
	{
		this->Property->StopListeningToProperty(this);
	}

	this->Property = Prop;

	if (this->Property)
	{
		FRPGPropertyChanged Callback;
		Callback.BindDynamic(this, &URPGResourceProgressBar::OnPropertyChanged);

		this->Property->ListenToProperty(Callback);
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
