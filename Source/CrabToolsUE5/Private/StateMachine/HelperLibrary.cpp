#include "StateMachine/HelperLibrary.h"
#include "StateMachine/EventListener.h"
#include "StateMachine/DataStructures.h"

#if !UE_BUILD_SHIPPING
	#include "Logging/MessageLog.h"
#endif

#define LOCTEXT_NAMESPACE "StateMachine"

void UStateMachineHelperLibrary::EmitEvent(UObject* Obj, FName InEvent, UObject* Source)
{
	#if UE_BUILD_SHIPPING
		IEventListenerInterface::Execute_Event(Obj, InEvent);
	#else
		if (IsValid(Obj))
		{
			IEventListenerInterface::Execute_Event(Obj, InEvent, Source);
		}
		else
		{
			FMessageLog Log("StateMachine");
			Log.Error(LOCTEXT("EmitEventError" , "Attempt to emit event to null State Machine"));
		}
	#endif	
}

void UStateMachineHelperLibrary::EmitEventSlot(UObject* Obj, FEventSlot InEvent, UObject* Source)
{
	EmitEvent(Obj, InEvent, Source);
}

void UStateMachineHelperLibrary::EmitEventWithData(UObject* Obj, FName InEvent, UObject* Data, UObject* Source)
{
	#if UE_BUILD_SHIPPING
		IEventListenerInterface::Execute_EventWithData(Obj, InEvent, Data);
	#else
		if (IsValid(Obj))
		{
			IEventListenerInterface::Execute_EventWithData(Obj, InEvent, Data, Source);
		}
		else
		{
			FMessageLog Log("StateMachine");
			Log.Error(LOCTEXT("EmitEventError", "Attempt to emit event to null State Machine"));
		}
	#endif
}

void UStateMachineHelperLibrary::EmitEventSlotWithData(UObject* Obj, FEventSlot InEvent, UObject* Data, UObject* Source)
{
	EmitEventWithData(Obj, InEvent, Data, Source);
}

#undef LOCTEXT_NAMESPACE