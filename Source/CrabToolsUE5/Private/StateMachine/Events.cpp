#include "StateMachine/Events.h"
#include "StateMachine/StateMachine.h"

FName Events::Default::DefaultEventToName(EDefaultStateMachineEvents Event)
{
    switch (Event)
    {
        case EDefaultStateMachineEvents::ON_MACHINE_DEACTIVATE: return Events::Default::MACHINE_DEACTIVATED;
        case EDefaultStateMachineEvents::ON_MACHINE_ACTIVATE: return Events::Default::MACHINE_ACTIVATED;
        default: return NAME_None;
    }
}
