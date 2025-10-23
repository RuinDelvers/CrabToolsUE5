#include "StateMachine/Transient/TransientNode.h"
#include "StateMachine/Transient/TransientTask.h"
#include "StateMachine/Events.h"

UTransientNode::UTransientNode()
{
	this->AddEmittedEvent(Events::Tasks::COMPLETED);
	this->AddEmittedEvent(Events::Tasks::NO_TASK);
}

void UTransientNode::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->CurrentTask)
	{
		this->CurrentTask->TickTask(DeltaTime);
	}
}

void UTransientNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Task = Cast<UTransientTask>(Data))
	{
		this->CurrentTask = Task;
		this->bHasFinishedTask = false;

		this->CurrentTask->BeginTask();

		this->CurrentTask->OnTaskCompleted.AddUFunction(
			this,
			GET_FUNCTION_NAME_CHECKED(UTransientNode, OnTaskFinished));
	}
}

void UTransientNode::Exit_Inner_Implementation()
{
	if (this->CurrentTask)
	{
		this->CurrentTask->OnTaskCompleted.RemoveAll(this);

		if (!this->bHasFinishedTask)
		{
			this->CurrentTask->EndTask();
		}
	}
}

bool UTransientNode::RequiresTick_Implementation() const
{
	if (this->CurrentTask)
	{
		return this->CurrentTask->RequiresTick();
	}
	else
	{
		return false;
	}
}

void UTransientNode::Enter_Inner_Implementation()
{
	if (this->CurrentTask)
	{
		this->CurrentTask->BeginTask();
	}
	else
	{
		this->EmitEvent(Events::Tasks::NO_TASK);
	}
}

void UTransientNode::OnTaskFinished()
{
	this->EmitEvent(Events::Tasks::COMPLETED);
}