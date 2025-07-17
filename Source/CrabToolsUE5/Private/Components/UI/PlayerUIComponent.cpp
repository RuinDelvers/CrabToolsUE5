#include "Components/UI/PlayerUIComponent.h"
#include "Utils/UtilsLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UPlayerUIComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(this->PlayerUIRoot))
	{
		this->InitWidget(this->PlayerUIRoot);
	}
}

void UPlayerUIComponent::InitWidget(UUserWidget* Widget)
{
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
}

void UPlayerUIComponent::SetWidgetClass(TSubclassOf<UUserWidget> WidgetClass)
{
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

	if (this->PlayerUIRoot)
	{
		this->InitWidget(this->PlayerUIRoot);
	}
}

void UPlayerUIComponent::ToggleBindings()
{
	if (this->bInputBound)
	{		
		UUtilsLibrary::UnbindObjectToEnhancedInput(this->PlayerUIRoot, Cast<APawn>(this->GetOwner()));
	}
	else
	{
		UUtilsLibrary::BindObjectToEnhancedInput(this->PlayerUIRoot, Cast<APawn>(this->GetOwner()));
	}
		
	this->bInputBound = !this->bInputBound;
}