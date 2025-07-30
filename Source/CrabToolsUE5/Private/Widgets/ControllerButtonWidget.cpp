#include "Widgets/ControllerButtonWidget.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "Styling/DefaultStyleCache.h"

UControllerButtonWidget::UControllerButtonWidget(const FObjectInitializer& ObjectInitializer)
{
	this->KeybindText = CreateDefaultSubobject<UTextBlock>(TEXT("KeybindText"));
	this->KBM_Default = UE::Slate::Private::FDefaultStyleCache::GetRuntime().GetButtonStyle();
	this->Gamepad_Default = UE::Slate::Private::FDefaultStyleCache::GetRuntime().GetButtonStyle();
}

void UControllerButtonWidget::OnWidgetRebuilt()
{
	this->AddChild(this->KeybindText);
	Super::OnWidgetRebuilt();

	auto System = UInputDeviceSubsystem::Get();	

	if (!this->IsDesignTime())
	{
		System->OnInputHardwareDeviceChanged.AddDynamic(this, &UControllerButtonWidget::OnHardwareDeviceChanged);
	}
}

void UControllerButtonWidget::OnHardwareDeviceChanged(const FPlatformUserId UserID, const FInputDeviceId DeviceID)
{
	auto System = UInputDeviceSubsystem::Get();

	auto HWDevice = System->GetMostRecentlyUsedHardwareDevice(UserID);

	this->UpdateInputType(HWDevice.PrimaryDeviceType);
}

void UControllerButtonWidget::UpdateInputType(EHardwareDevicePrimaryType Type)
{
	this->CurrentType = Type;

	switch (Type)
	{
	case EHardwareDevicePrimaryType::KeyboardAndMouse:
		this->SetStyle(this->KBM_Default);
		break;
	case EHardwareDevicePrimaryType::Gamepad:
		this->SetStyle(this->Gamepad_Default);
		break;
	default: break;
	}

	this->UpdateInputText();
}

void UControllerButtonWidget::UpdateInputText()
{
	if (IsValid(this->KeybindText))
	{
		switch (this->CurrentType)
		{
		case EHardwareDevicePrimaryType::KeyboardAndMouse:
			this->SetStyle(this->KBM_Default);
			this->KeybindText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		case EHardwareDevicePrimaryType::Gamepad:
			this->SetStyle(this->Gamepad_Default);
			this->KeybindText->SetVisibility(ESlateVisibility::Hidden);
			break;
		default: break;
		}
	}
}

#if WITH_EDITOR
void UControllerButtonWidget::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	if (Event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UControllerButtonWidget, CurrentType))
	{
		this->UpdateInputType(this->CurrentType);
	}
}

void UControllerButtonWidget::PostLinkerChange()
{
	Super::PostLinkerChange();

	this->UpdateInputType(this->CurrentType);
}
#endif