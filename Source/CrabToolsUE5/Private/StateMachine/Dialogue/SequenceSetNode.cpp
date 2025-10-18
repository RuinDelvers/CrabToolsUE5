#include "StateMachine/Dialogue/SequenceSetNode.h"
#include "LevelSequenceActor.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "StateMachine/Events.h"

USequenceSetNode::USequenceSetNode()
{
	this->AddEmittedEvent(Events::Dialogue::SEQUENCE_LOADED);
}

void USequenceSetNode::Initialize_Inner_Implementation()
{

}

void USequenceSetNode::PostTransition_Inner_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("PostTransition"));
	if (auto Actor = Cast<ALevelSequenceActor>(this->GetOwner()))
	{
		if (this->bAsyncLoad && this->Sequence.IsNull())
		{
			this->OnLoadStart();
			UAssetManager::GetStreamableManager().RequestAsyncLoad(
				this->Sequence.ToSoftObjectPath(),
				FStreamableDelegate::CreateUObject(this, &USequenceSetNode::OnLoadNative));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Sync Loading"));
			this->OnLoadStart();
			Actor->LevelSequenceAsset = this->Sequence.LoadSynchronous();
			this->OnLoadEnd();

			if (this->bPlayOnSet)
			{
				Actor->GetSequencePlayer()->Play();
			}

			this->EmitEvent(Events::Dialogue::SEQUENCE_LOADED);
		}	
	}
}

void USequenceSetNode::OnLoadNative()
{
	if (auto Actor = Cast<ALevelSequenceActor>(this->GetOwner()))
	{
		this->OnLoadEnd();
		if (this->bPlayOnSet)
		{
			Actor->GetSequencePlayer()->Play();
		}

		this->EmitEvent(Events::Dialogue::SEQUENCE_LOADED);
	}
}