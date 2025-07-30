#pragma once

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/InputSettings.h"
#include "ControllerButtonWidget.generated.h"

UCLASS(Blueprintable)
class UControllerButtonWidget : public UButton
{
	GENERATED_BODY()
	
private:

	/* Current hardware input type, also the default input type when the widget is built. */
	UPROPERTY(EditAnywhere, Category = "Hardware", meta = (AllowPrivateAccess, ShowInnerProperties))
	EHardwareDevicePrimaryType CurrentType = EHardwareDevicePrimaryType::KeyboardAndMouse;

	/* Textblock used to display KBM keybind (if any), hidden for gamepade inputs. */
	UPROPERTY(BlueprintReadOnly, Category="Keybinds", meta = (AllowPrivateAccess))
	TObjectPtr<UTextBlock> KeybindText;	

	UPROPERTY(EditAnywhere, Category="KBM", meta = (AllowPrivateAccess, ShowInnerProperties))
	FButtonStyle KBM_Default;

	UPROPERTY(EditAnywhere, Category = "KBM", meta=(AllowPrivateAccess, ShowInnerProperties))
	FKey KBM_KeyDefault;

	UPROPERTY(EditAnywhere, Category = "Gamepad", meta = (AllowPrivateAccess, ShowInnerProperties))
	FButtonStyle Gamepad_Default;

public:
	
	UControllerButtonWidget(const FObjectInitializer& ObjectInitializer);

	virtual void OnWidgetRebuilt() override;

protected:

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
		virtual void PostLinkerChange() override;
	#endif

private:

	UFUNCTION()
	void OnHardwareDeviceChanged(const FPlatformUserId UserID, const FInputDeviceId DeviceID);

	void UpdateInputType(EHardwareDevicePrimaryType Type);
	void UpdateInputText();

};
