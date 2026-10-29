#include "StateMachine/Input/InputNode.h"
#include "EnhancedInputComponent.h"


void UInputNode::Initialize_Inner_Implementation() {
	Super::Initialize_Inner_Implementation();

	if (!this->Action) { return; }

	if (IsValid(this->PawnOwner))
	{
		if (this->bAlwaysBound)
		{
			this->BindCallbacks();
		}
	}
}

void UInputNode::Enter_Inner_Implementation()
{
	if (!this->bAlwaysBound)
	{
		this->BindCallbacks();
	}
}

void UInputNode::Exit_Inner_Implementation()
{
	if (!this->bAlwaysBound)
	{
		this->UnbindCallbacks();
	}
}

void UInputNode::BindCallbacks()
{
	if (this->Component)
	{
		if (this->Events & static_cast<int32>(ETriggerEvent::Triggered))
		{
			this->Trigger = this->Component->BindAction(
				this->Action,
				ETriggerEvent::Triggered,
				this,
				&UInputNode::TriggerCallback_Internal).GetHandle();
		}

		if (this->Events & static_cast<int32>(ETriggerEvent::Started))
		{
			this->Start = this->Component->BindAction(
				this->Action,
				ETriggerEvent::Started,
				this,
				&UInputNode::StartCallback_Internal).GetHandle();
		}

		if (this->Events & static_cast<int32>(ETriggerEvent::Ongoing))
		{
			this->Ongoing = this->Component->BindAction(
				this->Action,
				ETriggerEvent::Ongoing,
				this,
				&UInputNode::TriggerCallback_Internal).GetHandle();
		}

		if (this->Events & static_cast<int32>(ETriggerEvent::Canceled))
		{
			this->Canceled = this->Component->BindAction(
				this->Action,
				ETriggerEvent::Canceled,
				this,
				&UInputNode::CanceledCallback_Internal).GetHandle();
		}
		else
		{
			this->Canceled = this->Component->BindAction(
				this->Action,
				ETriggerEvent::Canceled,
				this,
				&UInputNode::FinishedCallback).GetHandle();
		}

		if (this->Events & static_cast<int32>(ETriggerEvent::Completed))
		{
			this->Completed = this->Component->BindAction(
				this->Action,
				ETriggerEvent::Completed,
				this,
				&UInputNode::CompletedCallback_Internal).GetHandle();
		}
		else
		{
			this->Completed = this->Component->BindAction(
				this->Action,
				ETriggerEvent::Completed,
				this,
				&UInputNode::FinishedCallback).GetHandle();
		}
	}
}

void UInputNode::UnbindCallbacks()
{
	if (this->Component)
	{
		if (this->Events & static_cast<int32>(ETriggerEvent::Triggered))
		{
			this->Component->RemoveActionBindingForHandle(this->Trigger);
		}

		if (this->Events & static_cast<int32>(ETriggerEvent::Started))
		{
			this->Component->RemoveActionBindingForHandle(this->Start);
		}

		if (this->Events & static_cast<int32>(ETriggerEvent::Ongoing))
		{
			this->Component->RemoveActionBindingForHandle(this->Ongoing);
		}

		this->Component->RemoveActionBindingForHandle(this->Canceled);

		this->Component->RemoveActionBindingForHandle(this->Completed);
	}
}

void UInputNode::TriggerCallback_Implementation(const FInputActionValue& Value) {}

void UInputNode::TriggerCallback_Internal(const FInputActionValue& Value) {
	this->RecentValue = Value;
	if (this->Active()) {
		this->TriggerCallback(Value);
	}
}


void UInputNode::StartCallback_Implementation(const FInputActionValue& Value) {}

void UInputNode::StartCallback_Internal(const FInputActionValue& Value) {
	this->bHasStarted = true;
	this->RecentValue = Value;
	if (this->Active()) {
		this->StartCallback(Value);
	}
}

void UInputNode::OngoingCallback_Implementation(const FInputActionValue& Value) {}

void UInputNode::OngoingCallback_Internal(const FInputActionValue& Value) {
	this->bHasStarted = false;
	this->RecentValue = Value;
	if (this->Active()) {
		this->OngoingCallback(Value);
	}
}

void UInputNode::CompletedCallback_Implementation(const FInputActionValue& Value) {}

void UInputNode::CompletedCallback_Internal(const FInputActionValue& Value) {
	this->bHasStarted = false;
	this->RecentValue = Value;
	if (this->Active()) {
		this->CompletedCallback(Value);
	}
}

void UInputNode::CanceledCallback_Implementation(const FInputActionValue& Value) {}

void UInputNode::CanceledCallback_Internal(const FInputActionValue& Value) {
	this->RecentValue = Value;
	if (this->Active()) {
		this->CanceledCallback(Value);
	}
}

void UInputNode::FinishedCallback(const FInputActionValue& Value) {
	this->bHasStarted = false;
	this->RecentValue = Value;
}

FVector2D UInputNode::GetVec2D() {
	return this->RecentValue.Get<FVector2D>();
}