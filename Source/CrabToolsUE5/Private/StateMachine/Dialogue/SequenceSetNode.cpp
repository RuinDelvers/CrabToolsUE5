#include "StateMachine/Dialogue/SequenceSetNode.h"
#include "LevelSequenceActor.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "StateMachine/Events.h"
#include "LevelSequencePlayer.h"
#include "Properties/GenericPropertyBinding.h"

USequenceSetNode::USequenceSetNode()
{
	this->AddEmittedEvent(Events::Dialogue::SEQUENCE_LOADED);
	this->SeqBinding = CreateDefaultSubobject<UGenericPropertyBinding>(TEXT("SeqBinding"));
}

void USequenceSetNode::Initialize_Inner_Implementation()
{

}

void USequenceSetNode::Enter_Inner_Implementation()
{
	switch (this->SequenceSource)
	{
	case ESequenceSource::INLINE: this->LoadInline(); break;
	case ESequenceSource::PROPERTY: this->LoadProperty(); break;
	case ESequenceSource::INTERFACE: this->LoadInterface(); break;
	}
}

void USequenceSetNode::LoadInterface()
{
	switch (this->SequenceSource)
	{
		case ESequenceSource::INTERFACE:
		{
			auto Machine = this->GetMachine();
			if (Machine->Implements<UCutsceneStateMachine>())
			{
				this->LoadSoftPtr(ICutsceneStateMachine::Execute_GetMachineSequence(Machine));
			}
		}
		case ESequenceSource::PARENT_INTERFACE:
		{
			auto Machine = this->GetMachine()->GetParentMachine();
			if (Machine && Machine->Implements<UCutsceneStateMachine>())
			{
				this->LoadSoftPtr(ICutsceneStateMachine::Execute_GetMachineSequence(Machine));
			}
		}
	}
}

ESequenceStatePlayPhase USequenceSetNode::GetPlayPhase() const
{
	ESequenceStatePlayPhase Phase = ESequenceStatePlayPhase::UNSPECIFIED;
	auto Machine = this->GetMachine();

	if (Machine->Implements<UCutsceneStateMachine>())
	{
		Phase = ICutsceneStateMachine::Execute_GetPlayPhase(Machine);
	}
	else
	{
		if (auto Parent = Machine->GetParentMachine())
		{
			if (Parent->Implements<UCutsceneStateMachine>())
			{
				Phase = ICutsceneStateMachine::Execute_GetPlayPhase(Parent);
			}
		}
	}

	if (Phase == ESequenceStatePlayPhase::UNSPECIFIED)
	{
		return this->PlayPhase;
	}
	else
	{
		return Phase;
	}
}

void USequenceSetNode::LoadProperty()
{

}

void USequenceSetNode::LoadInline()
{
	this->LoadSoftPtr(this->Sequence);
}

void USequenceSetNode::LoadSoftPtr(TSoftObjectPtr<ULevelSequence> Ptr)
{
	if (auto Actor = Cast<ALevelSequenceActor>(this->GetOwner()))
	{
		if (this->bAsyncLoad && Ptr.IsNull())
		{
			this->OnLoadStart();
			UAssetManager::GetStreamableManager().RequestAsyncLoad(
				Ptr.ToSoftObjectPath(),
				FStreamableDelegate::CreateUObject(this, &USequenceSetNode::OnLoadNative, Ptr));
		}
		else
		{
			this->OnLoadStart();
			Actor->SetSequence(Ptr.LoadSynchronous());
			
			this->OnLoadEnd();

			if (this->GetPlayPhase() == ESequenceStatePlayPhase::ON_LOAD)
			{
				Actor->GetSequencePlayer()->Play();
			}

			this->EmitEvent(Events::Dialogue::SEQUENCE_LOADED);
		}
	}
}

void USequenceSetNode::Exit_Inner_Implementation()
{
	if (this->GetPlayPhase() == ESequenceStatePlayPhase::ON_EXIT)
	{
		if (auto Actor = Cast<ALevelSequenceActor>(this->GetOwner()))
		{
			Actor->GetSequencePlayer()->Play();
		}
	}
}

void USequenceSetNode::OnLoadNative(TSoftObjectPtr<ULevelSequence> Ptr)
{
	if (auto Actor = Cast<ALevelSequenceActor>(this->GetOwner()))
	{
		Actor->SetSequence(Ptr.Get());
		this->OnLoadEnd();

		if (this->GetPlayPhase() == ESequenceStatePlayPhase::ON_LOAD)
		{
			Actor->GetSequencePlayer()->Play();
		}

		this->EmitEvent(Events::Dialogue::SEQUENCE_LOADED);
	}
}