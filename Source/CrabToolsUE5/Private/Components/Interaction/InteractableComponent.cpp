#include "Components/Interaction/InteractableComponent.h"
#include "Components/Interaction/InteractionSystem.h"

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const auto& Names : this->Interactions)
	{
		if (this->GetOwner()->FindFunction(Names))
		{
			FInteraction Callback;
			FScriptDelegate Single;

			Single.BindUFunction(this->GetOwner(), Names);
			Callback.Add(Single);

			this->InteractionCallbacks.Add(Names, Callback);
		}
		else
		{
			FMessageLog Log("PIE");
			Log.Error(
				FText::FormatNamed(
					NSLOCTEXT("InteractableComponent", "MissingFunctionError", "Function {FnName} does not exist on actor {Owner}"),
					TEXT("FnName"), FText::FromName(Names),
					TEXT("Owner"), FText::FromString(this->GetOwner()->GetName())));
		}
	}

	this->GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UInteractableComponent::OnBeginActorOverlap);
	this->GetOwner()->OnActorEndOverlap.AddDynamic(this, &UInteractableComponent::OnEndActorOverlap);
}

void UInteractableComponent::InteractDefault(AActor* Interactor, UObject* Data)
{
	if (this->InteractionCallbacks.Contains(this->DefaultInteraction))
	{
		this->InteractionCallbacks[this->DefaultInteraction].Broadcast(Interactor, Data);
	}
}

void UInteractableComponent::Interact(FName Interaction, AActor* Interactor, UObject* Data)
{
	if (this->InteractionCallbacks.Contains(Interaction))
	{
		this->InteractionCallbacks[Interaction].Broadcast(Interactor, Data);
	}
}

void UInteractableComponent::EndInteraction(AActor* Interactor, UObject* Data)
{
	this->OnInteractionEnded.Broadcast(Interactor, Data);
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
		IntSys->AddInteractable(this);
	}
}

void UInteractableComponent::OnEndActorOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (auto IntSys = OtherActor->GetComponentByClass<UInteractionSystem>())
	{
		IntSys->RemoveInteractable(this);
	}
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
		Names.Add(Name.ToString());
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UInteractableComponent::GetInteractionOptions() const
{
	TArray<FString> OwnerInteractions;

	// Owner is null during blueprint editing, and the outer is its class when editing.
	if (auto Owner = Cast<UClass>(this->GetOuter()))
	{
		auto FnSign = this->FindFunctionChecked(GET_FUNCTION_NAME_CHECKED(UInteractableComponent, ExampleInteraction));

		for (TFieldIterator<UFunction> FIT(Owner, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			UFunction* f = *FIT;

			bool SignCompat = f->IsSignatureCompatibleWith(FnSign);
			bool IsPublic = !f->HasAnyFunctionFlags(EFunctionFlags::FUNC_Private | EFunctionFlags::FUNC_Protected);
			//bool Category = f->GetMetaData("Category").StartsWith("Interaction") || f->GetName().StartsWith("Interaction_");

			if (SignCompat && IsPublic)
			{
				OwnerInteractions.Add(f->GetName());
			}
		}
	}

	return OwnerInteractions;
}


#endif