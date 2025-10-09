#pragma once

#include "Components/RichTextBlock.h"
#include "GameplayTagContainer.h"
#include "LinkRichTextBlock.generated.h"

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UTagLinkListener : public UInterface
{
	GENERATED_BODY()
};

class ITagLinkListener
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintNativeEvent, Category = "UI|Links")
	void OnLinkClicked(FGameplayTag Link);
	virtual void OnLinkClicked_Implementation(FGameplayTag Link) {}

	UFUNCTION(BlueprintNativeEvent, Category = "UI|Links")
	void GetLinkStyling(FButtonStyle& OutStyle);
	virtual void GetLinkStyling_Implementation(FButtonStyle& OutStyle) {  }

	UFUNCTION(BlueprintNativeEvent, Category = "UI|Links")
	FLinearColor GetLinkColor();
	virtual FLinearColor GetLinkColor_Implementation() { return FLinearColor(0, 1, 0); }

	UFUNCTION(BlueprintNativeEvent, Category = "UI|Links")
	FLinearColor GetHoveredLinkColor();
	virtual FLinearColor GetHoveredLinkColor_Implementation() { return FLinearColor(0, 0, 1); }
};

/*
 * A RichTextBlock that can respond to links in the form of GameplayTags. Will broadcast
 * the links that are clicked and listeners can handle the links.
 */
UCLASS()
class ULinkRichTextBlock : public URichTextBlock, public ITagLinkListener
{
	GENERATED_BODY()

private:
	
public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLinkClicked, FGameplayTag, Tag);

	UPROPERTY(BlueprintAssignable, Category="Links")
	FLinkClicked OnLinkClicked;

	UPROPERTY(EditAnywhere, Category = "Mouse")
	FButtonStyle LinkStyling;

	UPROPERTY(EditAnywhere, Category = "Mouse", meta=(DesignerRebuild))
	FLinearColor LinkColor;

	UPROPERTY(EditAnywhere, Category = "Mouse", meta = (DesignerRebuild))
	FLinearColor LinkHoveredColor;

public:

	ULinkRichTextBlock(const FObjectInitializer& ObjectInitializer);

	virtual void OnLinkClicked_Implementation(FGameplayTag Link) override;
	virtual void GetLinkStyling_Implementation(FButtonStyle& OutStyle) override;
	virtual FLinearColor GetLinkColor_Implementation() override;
	virtual FLinearColor GetHoveredLinkColor_Implementation() override;

private:

	
};