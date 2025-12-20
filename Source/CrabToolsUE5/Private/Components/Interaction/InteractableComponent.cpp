#include "Components/Interaction/InteractableComponent.h"
#include "Components/Interaction/InteractionSystem.h"
#include "StateMachine/EventListener.h"
#include "StateMachine/Events.h"

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto& Names : this->Interactions)
	{
		if (this->GetOwner()->FindFunction(Names.Key))
		{
			FScriptDelegate Single;
			Single.BindUFunction(this->GetOwner(), Names.Key);

			Names.Value.Interaction.Add(Single);

			if (Names.Value.UIData.DisplayText.IsEmpty())
			{
				Names.Value.UIData.DisplayText = FText::FromString(FName::NameToDisplayString(Names.Key.ToString(), false));
			}
		}
		else
		{
			FMessageLog Log("PIE");
			Log.Error(
				FText::FormatNamed(
					NSLOCTEXT("InteractableComponent", "MissingFunctionError", "Function {FnName} does not exist on actor {Owner}"),
					TEXT("FnName"), FText::FromName(Names.Key),
					TEXT("Owner"), FText::FromString(this->GetOwner()->GetName())));
		}
	}

	this->GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UInteractableComponent::OnBeginActorOverlap);
	this->GetOwner()->OnActorEndOverlap.AddDynamic(this, &UInteractableComponent::OnEndActorOverlap);
}

void UInteractableComponent::InteractDefault(AActor* Interactor, UObject* Data)
{
	if (this->ValidActors.Contains(Interactor))
	{
		if (auto IntData = this->Interactions.Find(this->DefaultInteraction))
		{
			if (IntData->State.bIsEnabled)
			{
				IntData->Interaction.Broadcast(Interactor, Data);
			}
		}
	}
}

void UInteractableComponent::SetInteractionEnabled(FName Interaction, bool bNewEnabled)
{
	if (auto Data = this->Interactions.Find(Interaction))
	{
		Data->State.bIsEnabled = bNewEnabled;
	}
}

void UInteractableComponent::SetDefaultInteraction(FName NewDefaultInteraction)
{
	if (this->Interactions.Contains(NewDefaultInteraction))
	{
		this->DefaultInteraction = NewDefaultInteraction;
	}
}

void UInteractableComponent::Interact(FName Interaction, AActor* Interactor, UObject* Data)
{
	if (this->ValidActors.Contains(Interactor))
	{
		if (auto IntData = this->Interactions.Find(Interaction))
		{
			if (IntData->State.bIsEnabled)
			{
				IntData->Interaction.Broadcast(Interactor, Data);
			}
		}
	}
}

void UInteractableComponent::EndInteraction(AActor* Interactor, UObject* Data)
{
	this->OnInteractionEnded.Broadcast(Interactor, Data);
}

void UInteractableComponent::SetInteractionState(FName Interaction, const FInteractableStateData& NewData)
{
	if (auto Int = this->Interactions.Find(Interaction))
	{
		Int->State = NewData;
	}
}

void UInteractableComponent::GetInteractionPoints(TArray<FVector>& Points) const
{
	for (const auto& Children : this->GetAttachChildren())
	{
		Points.Add(Children->GetComponentLocation());
	}
}

void UInteractableComponent::OnBeginActorOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (auto IntSys = OtherActor->GetComponentByClass<UInteractionSystem>())
	{
		this->ValidActors.Add(OtherActor);
		IntSys->AddInteractable(this);
	}
}

void UInteractableComponent::OnEndActorOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (auto IntSys = OtherActor->GetComponentByClass<UInteractionSystem>())
	{
		this->ValidActors.Remove(OtherActor);
		IntSys->RemoveInteractable(this);
	}
}

TArray<UContextMenuEntry*> UInteractableComponent::GatherEntries_Implementation(AActor* Requester)
{
	TArray<UContextMenuEntry*> Entries;

	for (const auto& Data : this->Interactions)
	{
		if (Data.Value.State.bIsEnabled)
		{
			auto Entry = NewObject<UContextMenuEntry>(this);
			Entry->OnActionNative.AddUFunction(
				this,
				Data.Value.bUseMoveLogic
				? GET_FUNCTION_NAME_CHECKED(UInteractableComponent, MoveToInteract)
				: GET_FUNCTION_NAME_CHECKED(UInteractableComponent, Interact),
				Data.Key,
				Requester,
				nullptr);

			Entry->SetDisplayData(Data.Value.UIData);

			Entries.Add(Entry);
		}
	}

	return Entries;
}

void UInteractableComponent::MoveToInteract(FName Interaction, AActor* Interactor, UObject* Data)
{
	auto AIData = UAIInteractionData::MakeInteractionData(Interaction, this->GetOwner(), Data);
	IEventListenerInterface::Execute_EventWithData(Interactor, this->MoveLogicEvent, AIData);
}

TArray<FString> UInteractableComponent::GetActorInteractions(UClass* Class)
{
	TArray<FString> OwnerInteractions;

	if (Class)
	{
		auto FnSign = UInteractableComponent::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UInteractableComponent, ExampleInteraction));

		for (TFieldIterator<UFunction> FIT(Class, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			UFunction* f = *FIT;

			bool SignCompat = f->IsSignatureCompatibleWith(FnSign);
			bool IsPublic = !f->HasAnyFunctionFlags(EFunctionFlags::FUNC_Private | EFunctionFlags::FUNC_Protected);

			if (SignCompat && IsPublic)
			{
				OwnerInteractions.Add(f->GetName());
			}
		}
	}

	return OwnerInteractions;
}

#if WITH_EDITOR

void UInteractableComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!this->Interactions.Contains(this->DefaultInteraction))
	{
		this->DefaultInteraction = NAME_None;
	}
}

TArray<FString> UInteractableComponent::GetDefaultOptions() const
{
	TArray<FString> Names;

	for (const auto& Name : this->Interactions)
	{
		Names.Add(Name.Key.ToString());
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UInteractableComponent::GetInteractionOptions() const
{
	auto Owner = IsValid(this->SourceActor) ? this->SourceActor->GetClass() : Cast<UClass>(this->GetOuter());

	if (Owner)
	{
		auto Names = GetActorInteractions(Owner);

		for (const auto& Data : this->Interactions)
		{
			Names.Remove(Data.Key.ToString());
		}

		return Names;
	}
	else
	{
		return {};
	}
}

#endif

UAIInteractionData* UAIInteractionData::MakeInteractionData(
	FName InitInteraction,
	AActor* InitInteractable,
	UObject* InitData)
{
	auto Obj = NewObject<UAIInteractionData>(InitInteractable);

	Obj->Interaction = InitInteraction;
	Obj->Interactable = InitInteractable;
	Obj->Data = InitData;

	return Obj;
}

bool UAIInteractionData::IsValidData() const
{
	bool BaseValid = IsValid(this->Interactable);

	if (BaseValid)
	{
		return IsValid(this->Interactable->GetComponentByClass<UInteractableComponent>());
	}
	else
	{
		return false;
	}
}