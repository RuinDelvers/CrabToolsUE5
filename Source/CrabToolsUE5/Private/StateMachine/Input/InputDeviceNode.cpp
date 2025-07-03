#include "StateMachine/Input/InputDeviceNode.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "StateMachine/Events.h"

void UInputDeviceNode::Initialize_Inner_Implementation()
{
	auto System = GEngine->GetEngineSubsystem<UInputDeviceSubsystem>();
	System->OnInputHardwareDeviceChanged.AddDynamic(this, &UInputDeviceNode::OnDeviceChanged);
}

void UInputDeviceNode::OnDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId)
{
	if (this->Active())
	{
		auto System = GEngine->GetEngineSubsystem<UInputDeviceSubsystem>();

		this->EmitEvent(
			Events::Input::Devices::DeviceToEvent(
				System->GetMostRecentlyUsedHardwareDevice(UserId).PrimaryDeviceType));
	}
}

#if WITH_EDITOR
void UInputDeviceNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Events.Add(Events::Input::Devices::INPUT_DEVICE_UNSPECIFIED);
	Events.Add(Events::Input::Devices::INPUT_DEVICE_KEYBOARD_AND_MOUSE);
	Events.Add(Events::Input::Devices::INPUT_DEVICE_GAMEPAD);
	Events.Add(Events::Input::Devices::INPUT_DEVICE_TOUCH);
	Events.Add(Events::Input::Devices::INPUT_DEVICE_MOTION_TRACKING);
	Events.Add(Events::Input::Devices::INPUT_DEVICE_RACING_WHEEL);
	Events.Add(Events::Input::Devices::INPUT_DEVICE_FLIGHT_STICK);
	Events.Add(Events::Input::Devices::INPUT_DEVICE_INSTRUMENT);
	Events.Add(Events::Input::Devices::INPUT_DEVICE_CUSTOM_TYPE_A);
	Events.Add(Events::Input::Devices::INPUT_DEVICE_CUSTOM_TYPE_B);
	Events.Add(Events::Input::Devices::INPUT_DEVICE_CUSTOM_TYPE_C);
	Events.Add(Events::Input::Devices::INPUT_DEVICE_CUSTOM_TYPE_D);
}
#endif