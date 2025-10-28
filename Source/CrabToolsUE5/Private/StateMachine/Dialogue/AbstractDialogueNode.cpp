#include "StateMachine/Dialogue/AbstractDialogueNode.h"
#include "Logging/MessageLog.h"

void UAbstractDialogueNode::Initialize_Inner_Implementation()
{
	this->DialogueComponent = this->GetActorOwner()->GetComponentByClass<UDialogueStateComponent>();

	if (!IsValid(this->DialogueComponent))
	{
		auto ErrorMessage = NSLOCTEXT("UMonologueNode", "MissingDialogueComponent", "Dialogue component not available from actor: {Actor}");
		ErrorMessage = FText::FormatNamed(
			ErrorMessage,
			TEXT("Actor"),
			FText::FromString(this->GetOwner()->GetName()));

		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage.ToString());

		auto Log = FMessageLog("StateMachineErrors");
		Log.Open(EMessageSeverity::Error, true);
		Log.Message(EMessageSeverity::Error, ErrorMessage);
	}
}