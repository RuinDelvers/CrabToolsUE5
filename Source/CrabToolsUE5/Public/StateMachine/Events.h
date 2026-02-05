#pragma once

/*
 * This file defines various namespaces for events that are natively used for nodes and
 * state machines. 
 */

#include "GameFramework/InputSettings.h"

enum class EDEfaultStateMachineEvents;

namespace Events::Input::Devices
{
	// See EHardwareDevicePrimaryType in InputSettings.h for the device types.
	constexpr char INPUT_DEVICE_UNSPECIFIED[]        = "INPUT_DEVICE_UNSPECIFIED";
	constexpr char INPUT_DEVICE_KEYBOARD_AND_MOUSE[] = "INPUT_DEVICE_KEYBOARD_AND_MOUSE";
	constexpr char INPUT_DEVICE_GAMEPAD[]            = "INPUT_DEVICE_GAMEPAD";
	constexpr char INPUT_DEVICE_TOUCH[]              = "INPUT_DEVICE_TOUCH";
	constexpr char INPUT_DEVICE_MOTION_TRACKING[]    = "INPUT_DEVICE_MOTION_TRACKING";
	constexpr char INPUT_DEVICE_RACING_WHEEL[]       = "INPUT_DEVICE_RACING_WHEEL";
	constexpr char INPUT_DEVICE_FLIGHT_STICK[]       = "INPUT_DEVICE_FLIGHT_STICK";
	constexpr char INPUT_DEVICE_CAMERA[]             = "INPUT_DEVICE_CAMERA";
	constexpr char INPUT_DEVICE_INSTRUMENT[]         = "INPUT_DEVICE_INSTRUMENT";
	constexpr char INPUT_DEVICE_CUSTOM_TYPE_A[]     = "INPUT_DEVICE_CUSTOM_TYPE_A";
	constexpr char INPUT_DEVICE_CUSTOM_TYPE_B[]     = "INPUT_DEVICE_CUSTOM_TYPE_B";
	constexpr char INPUT_DEVICE_CUSTOM_TYPE_C[]     = "INPUT_DEVICE_CUSTOM_TYPE_C";
	constexpr char INPUT_DEVICE_CUSTOM_TYPE_D[]     = "INPUT_DEVICE_CUSTOM_TYPE_D";

	static FName DeviceToEvent(EHardwareDevicePrimaryType DType)
	{
		switch (DType)
		{
			case EHardwareDevicePrimaryType::Unspecified: return INPUT_DEVICE_UNSPECIFIED;
			case EHardwareDevicePrimaryType::KeyboardAndMouse: return INPUT_DEVICE_KEYBOARD_AND_MOUSE;
			case EHardwareDevicePrimaryType::Gamepad: return INPUT_DEVICE_GAMEPAD;
			case EHardwareDevicePrimaryType::Touch: return INPUT_DEVICE_TOUCH;
			case EHardwareDevicePrimaryType::MotionTracking: return INPUT_DEVICE_MOTION_TRACKING;
			case EHardwareDevicePrimaryType::RacingWheel: return INPUT_DEVICE_RACING_WHEEL;
			case EHardwareDevicePrimaryType::FlightStick: return INPUT_DEVICE_FLIGHT_STICK;
			case EHardwareDevicePrimaryType::Camera: return INPUT_DEVICE_CAMERA;
			case EHardwareDevicePrimaryType::Instrument: return INPUT_DEVICE_INSTRUMENT;
			case EHardwareDevicePrimaryType::CustomTypeA: return INPUT_DEVICE_CUSTOM_TYPE_A;
			case EHardwareDevicePrimaryType::CustomTypeB: return INPUT_DEVICE_CUSTOM_TYPE_B;
			case EHardwareDevicePrimaryType::CustomTypeC: return INPUT_DEVICE_CUSTOM_TYPE_C;
			case EHardwareDevicePrimaryType::CustomTypeD: return INPUT_DEVICE_CUSTOM_TYPE_D;
			default: return INPUT_DEVICE_UNSPECIFIED;
		}
	}
}

namespace Events::Default
{
	constexpr char MACHINE_ACTIVATED[] = "MACHINE_ACTIVATED";
	constexpr char MACHINE_DEACTIVATED[] = "MACHINE_DEACTIVATED";

	CRABTOOLSUE5_API FName DefaultEventToName(EDefaultStateMachineEvents Event);
}

namespace Events::AI
{
	// Event for when a path following entity has arrived at their desitnation.
	constexpr char ARRIVE[] = "AI_ON_ARRIVE";

	// Event for when a path following entity cannot reach their desitnation for some reason.
	constexpr char LOST[] = "AI_ON_LOST";

	// Event for when a wait has finished.
	constexpr char WAIT_FINISHED[] = "AI_ON_WAIT_FINISHED";

	// Event for when an ability has finished.
	constexpr char ABILITY_FINISHED[] = "AI_ABILITY_FINISHED";

	// Event for when a target cannot be interacted with.
	constexpr char CANNOT_INTERACT[] = "AI_CANNOT_INTERACT";

	// Event for when data is passed that doesn't implement that TargetingControllerInterface.
	constexpr char INVALID_TARGETING[] = "AI_INVALID_TARGETING";

	// Event for when targets have been confirmed by the targeting controller.
	constexpr char TARGETS_CONFIRMED[] = "AI_CONFIRMED_TARGETS";

	// Generic event for when something is done, but is a non-specific task.
	constexpr char DONE[] = "AI_DONE";

	// Event used by Interactables to automate move to and interact logic.
	constexpr char MOVE_AND_INTERACT[] = "MOVE_AND_INTERACT";
}

namespace Events::Animation
{
	// Event for when a path following entity has arrived at their desitnation.
	constexpr char NOTIFY_FINISH_WAIT[] = "ANIM_NOTIFY_FINISH_WAIT";
}

namespace Events::Tasks
{
	// Event for when a task has been completed.
	constexpr char COMPLETED[] = "TASK_COMPLETED";

	// Event emitted when no task has been received.
	constexpr char NO_TASK[] = "TASK_NULL";
}

namespace Events::Dialogue
{
	constexpr char REQUEST_CONFIRMED[]      = "REQUEST_DIALOGUE";
	constexpr char DIALOGUE_CONFIRMED[]     = "DIALOGUE_CONFIRMED";
	constexpr char DIALOGUE_FINISHED[]      = "DIALOGUE_FINISHED";
	constexpr char DIALOGUE_FAILED[]        = "DIALOGUE_FAILED";
	constexpr char MONOLOGUE_FINISHED[]     = "MONOLOGUE_FINISHED";
	constexpr char SEQUENCE_LOADED[]        = "SEQUENCE_LOADED";
	constexpr char NULL_SEQUENCE_PROGRESS[] = "PROGRESS_DIALOGUE";
}