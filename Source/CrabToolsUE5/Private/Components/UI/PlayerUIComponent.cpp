#include "Components/UI/PlayerUIComponent.h"
#include "Utils/UtilsLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UPlayerUIComponent::BeginPlay()
{
	Super::BeginPlay();

	#if !UE_SERVER
		if (IsValid(this->PlayerUIRoot))
		{
			this->InitWidget(this->PlayerUIRoot);
		}
	#endif
}

void UPlayerUIComponent::InitWidget(UUserWidget* Widget)
{
	#if !UE_SERVER
		if (auto Pawn = Cast<APawn>(this->GetOwner()))
		{
			if (auto PC = Pawn->GetLocalViewingPlayerController())
			{
				Widget->SetOwningPlayer(PC);
				Widget->Initialize();
			}
		}

		if (this->bAutomaticallyToViewport)
		{
			Widget->AddToPlayerScreen(this->ViewportZOrder);
		}

		IPlayerUIInterface::Execute_PostInitialize(Widget);
	#endif
}

void UPlayerUIComponent::SetWidgetClass(TSubclassOf<UUserWidget> WidgetClass)
{
	#if !UE_SERVER
		if (this->PlayerUIRoot)
		{
			this->PlayerUIRoot->RemoveFromParent();
			this->PlayerUIRoot = nullptr;
		}


		if (auto Pawn = Cast<APawn>(this->GetOwner()))
		{
			if (auto PC = Pawn->GetLocalViewingPlayerController())
			{
				this->PlayerUIRoot = UWidgetBlueprintLibrary::Create(nullptr, WidgetClass, PC);
			}
		}
		else if (auto PC = Cast<APlayerController>(this->GetOwner()))
		{
			this->PlayerUIRoot = UWidgetBlueprintLibrary::Create(nullptr, WidgetClass, PC);
		}

		if (this->PlayerUIRoot)
		{
			this->InitWidget(this->PlayerUIRoot);
		}
	#endif
}

void UPlayerUIComponent::ToggleBindings()
{
	#if !UE_SERVER
		if (this->bInputBound)
		{		
			UUtilsLibrary::UnbindObjectToEnhancedInput(this->PlayerUIRoot, Cast<APawn>(this->GetOwner()));
		}
		else
		{
			UUtilsLibrary::BindObjectToEnhancedInput(this->PlayerUIRoot, Cast<APawn>(this->GetOwner()));
		}
		
		this->bInputBound = !this->bInputBound;
	#endif
}

UWidget* UPlayerUIComponent::GetUIAs(TSubclassOf<UWidget> WidgetClass, bool& bFound)
{
	#if !UE_SERVER
		if (this->PlayerUIRoot && this->PlayerUIRoot->IsA(WidgetClass))
		{
			bFound = true;
			return this->PlayerUIRoot;
		}
		else
		{
			bFound = false;
			return nullptr;
		}
	#else
		return nullptr;
	#endif
}