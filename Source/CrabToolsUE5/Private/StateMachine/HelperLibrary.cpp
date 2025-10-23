#include "StateMachine/HelperLibrary.h"
#include "StateMachine/EventListener.h"
#include "StateMachine/DataStructures.h"

#if !UE_BUILD_SHIPPING
	#include "Logging/MessageLog.h"
#endif

#define LOCTEXT_NAMESPACE "StateMachine"

void UStateMachineHelperLibrary::EmitEvent(UObject* Obj, FName InEvent)
{
	#if UE_BUILD_SHIPPING
		IEventListenerInterface::Execute_Event(Obj, InEvent);
	#else
		if (IsValid(Obj))
		{
			IEventListenerInterface::Execute_Event(Obj, InEvent);
		}
		else
		{
			FMessageLog Log("StateMachine");
			Log.Error(LOCTEXT("EmitEventError" , "Attempt to emit event to null State Machine"));
		}
	#endif	
}

void UStateMachineHelperLibrary::EmitEventSlot(UObject* Obj, FEventSlot InEvent)
{
	EmitEvent(Obj, InEvent);
}

void UStateMachineHelperLibrary::EmitEventWithData(UObject* Obj, FName InEvent, UObject* Data)
{
	#if UE_BUILD_SHIPPING
		IEventListenerInterface::Execute_EventWithData(Obj, InEvent, Data);
	#else
		if (IsValid(Obj))
		{
			IEventListenerInterface::Execute_EventWithData(Obj, InEvent, Data);
		}
		else
		{
			FMessageLog Log("StateMachine");
			Log.Error(LOCTEXT("EmitEventError", "Attempt to emit event to null State Machine"));
		}
	#endif
}

void UStateMachineHelperLibrary::EmitEventSlotWithData(UObject* Obj, FEventSlot InEvent, UObject* Data)
{
	EmitEventWithData(Obj, InEvent, Data);
}

#undef LOCTEXT_NAMESPACE