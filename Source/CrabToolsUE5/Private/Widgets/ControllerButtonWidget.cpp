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
	Super::OnWidgetRebuilt();

	auto System = UInputDeviceSubsystem::Get();

	this->AddChild(this->KeybindText);

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
	switch (Type)
	{
	case EHardwareDevicePrimaryType::KeyboardAndMouse:
		this->SetStyle(this->KBM_Default);
		//this->KeybindText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case EHardwareDevicePrimaryType::Gamepad:
		this->SetStyle(this->Gamepad_Default);
		//this->KeybindText->SetVisibility(ESlateVisibility::Hidden);
		break;
	default: break;
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
#endif