#include "StateMachine/General/NodeWithChild.h"

void UNodeWithChild::Initialize_Inner_Implementation()
{
	if (this->Child)
	{
		this->Child->Initialize(this->GetMachine());
	}
}

void UNodeWithChild::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->Child)
	{
		this->Child->Tick(DeltaTime);
	}
}

void UNodeWithChild::Event_Inner_Implementation(FName Event)
{
	if (this->Child)
	{
		this->Child->Event(Event);
	}
}
void UNodeWithChild::EventWithData_Inner_Implementation(FName InEvent, UObject* Data)
{
	if (this->Child)
	{
		this->Child->EventWithData(InEvent, Data);
	}
}

void UNodeWithChild::Enter_Inner_Implementation()
{
	if (this->Child)
	{
		this->Child->Enter();
	}
}

void UNodeWithChild::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (this->Child)
	{
		this->Child->EnterWithData(Data);
	}
}

void UNodeWithChild::Exit_Inner_Implementation()
{
	if (this->Child)
	{
		this->Child->Exit();
	}
}

void UNodeWithChild::ExitWithData_Inner_Implementation(UObject* Data)
{
	if (this->Child)
	{
		this->Child->ExitWithData(Data);
	}
}

bool UNodeWithChild::RequiresTick_Implementation() const
{
	if (this->Child)
	{
		return this->Child->RequiresTick();
	}
	else
	{
		return false;
	}
}

bool UNodeWithChild::HasPipedData_Implementation() const
{
	if (this->Child)
	{
		return this->Child->HasPipedData();
	}
	else
	{
		return false;
	}
}

UObject* UNodeWithChild::GetPipedData_Implementation()
{
	if (this->Child)
	{
		return this->Child->GetPipedData();
	}
	else
	{
		return nullptr;
	}
}

void UNodeWithChild::SetActive_Inner_Implementation(bool bNewActive)
{
	if (this->Child)
	{
		this->Child->SetActive(bNewActive);
	}
}

#if WITH_EDITORONLY_DATA
void UNodeWithChild::GetEmittedEvents(TSet<FName>& Events) const
{
	if (this->Child)
	{
		this->Child->GetEmittedEvents(Events);
	}
}
#endif