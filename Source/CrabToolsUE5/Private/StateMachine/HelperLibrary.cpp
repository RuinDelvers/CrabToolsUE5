#include "StateMachine/HelperLibrary.h"
#include "StateMachine/EventListener.h"
#include "StateMachine/DataStructures.h"

#if !UE_BUILD_SHIPPING
	#include "Logging/MessageLog.h"
#endif

#define LOCTEXT_NAMESPACE "StateMachine"

void UStateMachineHelperLibrary::EmitEvent(UObject* Obj, FName EName)
{
	#if UE_BUILD_SHIPPING
		IEventListenerInterface::Execute_Event(Obj, EName);
	#else
		if (IsValid(Obj))
		{
			IEventListenerInterface::Execute_Event(Obj, EName);
		}
		else
		{
			FMessageLog Log("StateMachine");
			Log.Error(LOCTEXT("EmitEventError" , "Attempt to emit event to null State Machine"));
		}
	#endif	
}

void UStateMachineHelperLibrary::EmitEventSlot(UObject* Obj, FEventSlot EName)
{
	EmitEvent(Obj, EName);
}

void UStateMachineHelperLibrary::EmitEventWithData(UObject* Obj, FName EName, UObject* Data)
{
	#if UE_BUILD_SHIPPING
		IEventListenerInterface::Execute_EventWithData(Obj, EName, Data);
	#else
		if (IsValid(Obj))
		{
			IEventListenerInterface::Execute_EventWithData(Obj, EName, Data);
		}
		else
		{
			FMessageLog Log("StateMachine");
			Log.Error(LOCTEXT("EmitEventError", "Attempt to emit event to null State Machine"));
		}
	#endif
}

void UStateMachineHelperLibrary::EmitEventSlotWithData(UObject* Obj, FEventSlot EName, UObject* Data)
{
	EmitEventWithData(Obj, EName, Data);
}

#undef LOCTEXT_NAMESPACE