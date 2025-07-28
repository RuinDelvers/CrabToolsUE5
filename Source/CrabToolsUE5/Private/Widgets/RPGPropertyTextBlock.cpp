#include "Widgets/RPGPropertyTextBlock.h"
#include "Components/RPGSystem/RPGProperty.h"

void URPGPropertyTextBlock::SetProperty(URPGProperty* Prop)
{
	if (this->Property)
	{
		Property->StopListeningToProperty(this);
	}

	this->Property = Prop;

	if (this->Property)
	{
		FRPGPropertyChanged Event;
		Event.BindDynamic(this, &URPGPropertyTextBlock::OnPropertyChanged);

		this->Property->ListenToProperty(Event);

		this->OnPropertyChanged(Prop);
	}
}

void URPGPropertyTextBlock::OnPropertyChanged(URPGProperty* Prop)
{
	this->SetText(this->Property->GetDisplayText());
}