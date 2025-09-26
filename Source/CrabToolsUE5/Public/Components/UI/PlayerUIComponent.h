#pragma once

#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIComponent.generated.h"

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UPlayerUIInterface : public UInterface
{
	GENERATED_BODY()
};

class IPlayerUIInterface
{
	GENERATED_BODY()

public:

	/*
	 * This is called after the widget has been added by the PlayerUIComponent. This is often
	 * needed because most sub widgets aren't fully constructed when initialization occurs
	 * during Initialization or on Construct.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void PostInitialize();
};

UCLASS(Blueprintable, CollapseCategories, ClassGroup = (Custom),
	meta = (BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UPlayerUIComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI",
		meta=(AllowPrivateAccess, ShowInnerProperties))
	TObjectPtr<UUserWidget> PlayerUIRoot;

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess))
	bool bAutomaticallyToViewport = true;

	/* Whether or not this widget has been bound to the enhanced input system. Default is true */
	bool bInputBound = true;

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (AllowPrivateAccess))
	int ViewportZOrder = 0;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="UI",
		meta=(DeterminesOutputType="WidgetClass", ExpandBoolAsExecs="bFound"))
	UWidget* GetUIAs(TSubclassOf<UWidget> WidgetClass, bool& bFound);

	UFUNCTION(BlueprintCallable, Category="UI")
	void ToggleBindings();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetWidgetClass(TSubclassOf<UUserWidget> WidgetClass);

protected:

	virtual void BeginPlay() override;

	void InitWidget(UUserWidget* Widget);
};