#include "StateMachine/Input/MultiInputEventNode.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "Engine/InputDelegateBinding.h"


void UMultiInputEventNode::BindInput()
{
	for (const auto& Pair : this->EventMap)
	{
		for (const auto& EventPair : Pair.Value.Events)
		{
			if (!EventPair.Value.EventName.IsNone())
			{
				auto Handle = this->Component->BindAction(
					Pair.Key,
					EventPair.Key,
					this,
					&UMultiInputEventNode::EmitDefinedEvent,
					EventPair.Value,
					Pair.Key.Get()).GetHandle();
				this->Handles.Add(Handle);
			}
		}
	}
}

void UMultiInputEventNode::UnbindInput()
{
	for (uint32 Handle : this->Handles)
	{
		this->Component->RemoveActionBindingForHandle(Handle);
	}
	this->Handles.Empty();
}

void UMultiInputEventNode::SetActive_Inner_Implementation(bool bNewActive)
{
	if (bNewActive)
	{
		this->BindInput();
	}
	else
	{
		this->UnbindInput();
	}
}

void UMultiInputEventNode::EmitDefinedEvent(const FInputActionValue& Value, FEventDataStruct Data, UInputAction* Action)
{
	auto& MapData = this->EventMap[Action];

	if (MapData.LastFrame >= GFrameCounter)
	{
		return;
	}
	else
	{
		MapData.LastFrame = GFrameCounter;

		if (Data.Data)
		{
			this->EmitEventWithData(Data.EventName, Data.Data);
		}
		else if (Data.DataGenerator)
		{
			this->EmitEventWithData(
				Data.EventName,
				Data.DataGenerator->GetData(this->GetMachine()));
		}
		else
		{
			this->EmitEvent(Data.EventName);
		}
	}
}

#if WITH_EDITOR
void UMultiInputEventNode::GetEmittedEvents(TSet<FName>& Events) const
{
	for (const auto& Pair : this->EventMap)
	{
		for (const auto& EventPair : Pair.Value.Events)
		{
			if (!EventPair.Value.EventName.IsNone())
			{
				Events.Add(EventPair.Value.EventName);
			}
		}
	}
}
#endif