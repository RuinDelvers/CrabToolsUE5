#include "Animation/CustomizableAnimInstance.h"

void UCustomizableAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	this->Customization.PushUpdates();
}

TSet<FName> UCustomizableAnimInstance::GetAlphaCurveKeys_Implementation() const
{
	TSet<FName> Keys;

	this->Customization.FloatListeners.GetKeys(Keys);

	return Keys;
}

void UCustomizableAnimInstance::RegisterFloatValue(FName ValueKey, const FCustomizableFloatListener& Callback)
{
	this->Customization.RegisterFloatValue(ValueKey, Callback);
}

void UCustomizableAnimInstance::UpdateFloatValue_Implementation(FName ValueKey, float NewValue)
{
	this->Customization.UpdateFloatValue(ValueKey, NewValue);
}

void FCustomizableValueContainer::PushUpdates()
{
	for (const auto& Data : this->UpdatedFloatListeners)
	{
		this->FloatListeners[Data.Key].Broadcast(Data.Value);
	}

	this->ClearUpdates();
}

void FCustomizableValueContainer::ClearUpdates()
{
	this->UpdatedFloatListeners.Empty();
}

void FCustomizableValueContainer::RegisterFloatValue(FName ValueKey, const FCustomizableFloatListener& Callback)
{
	if (auto Listeners = this->FloatListeners.Find(ValueKey))
	{
		Listeners->Add(Callback);
	}
	else
	{
		FCustomizableFloatListener_Multi NewListeners;
		NewListeners.Add(Callback);

		this->FloatListeners.Add(ValueKey, NewListeners);
	}
}

void FCustomizableValueContainer::UpdateFloatValue(FName ValueKey, float NewValue)
{
	if (this->FloatListeners.Contains(ValueKey))
	{
		this->UpdatedFloatListeners.Add(ValueKey, NewValue);
	}
}
