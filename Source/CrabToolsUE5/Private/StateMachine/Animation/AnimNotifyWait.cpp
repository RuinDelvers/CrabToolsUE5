#include "StateMachine/Animation/AnimNotifyWait.h"
#include "Components/SkeletalMeshComponent.h"
#include "StateMachine/Events.h"
#include "Utils/UtilsLibrary.h"
#include "GameFramework/Character.h"
#include "StateMachine/Logging.h"

UAnimNotifyWaitNode::UAnimNotifyWaitNode()
{
	this->AddEmittedEvent(Events::Animation::NOTIFY_FINISH_WAIT);
	#if WITH_EDITORONLY_DATA
		this->ActorClass = ACharacter::StaticClass();
	#endif
}

void UAnimNotifyWaitNode::Initialize_Inner_Implementation()
{

	bool bFoundComp = false;

	for (auto& Components : this->GetActorOwner()->GetComponents())
	{
		if (auto SkComp = Cast<USkeletalMeshComponent>(Components))
		{
			if (Components->GetFName() == this->ComponentName) {
				this->Component = SkComp;
				bFoundComp = true;
				break;
			}
		}
	}

	if (!bFoundComp)
	{
		UE_LOG(LogStateMachine, Error, TEXT("Failed to find SkeletalMeshComponent %s from object %s"),
			*this->ComponentName.ToString(),
			*this->GetOwner()->GetName());
	}
}

void UAnimNotifyWaitNode::Enter_Inner_Implementation()
{
	if (this->Component)
	{
		if (auto AnimInst = this->Component->GetAnimInstance())
		{
			AnimInst->AddExternalNotifyHandler(
				this,
				GET_FUNCTION_NAME_CHECKED(UAnimNotifyWaitNode, AnimNotify_SM_FinishWaitState));
		}
	}
}

void UAnimNotifyWaitNode::Exit_Inner_Implementation()
{
	if (this->Component)
	{
		if (auto AnimInst = this->Component->GetAnimInstance())
		{
			AnimInst->RemoveExternalNotifyHandler(
				this,
				GET_FUNCTION_NAME_CHECKED(UAnimNotifyWaitNode, AnimNotify_SM_FinishWaitState));
		}
	}
}

#if WITH_EDITOR
TArray<FString> UAnimNotifyWaitNode::GetComponentOptions() const
{
	TArray<FString> Names;

	if (this->ActorClass)
	{
		for (auto& SkComp : UtilsFunctions::GetComponentsByClass<USkeletalMeshComponent>(this->ActorClass))
		{
			Names.Add(SkComp->GetName());
		}

		Names.Sort([&](const FString& A, const FString& B) { return A < B; });
	}

	return Names;
}
#endif //WITH_EDITOR

void UAnimNotifyWaitNode::AnimNotify_SM_FinishWaitState()
{
	this->EmitEvent(Events::Animation::NOTIFY_FINISH_WAIT);
}