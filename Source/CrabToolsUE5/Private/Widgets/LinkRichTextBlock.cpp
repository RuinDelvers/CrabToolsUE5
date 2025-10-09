#include "Widgets/LinkRichTextBlock.h"

ULinkRichTextBlock::ULinkRichTextBlock(const FObjectInitializer& ObjectInitializer)
: URichTextBlock(ObjectInitializer),
	LinkColor(FLinearColor(0, 1, 0)),
	LinkHoveredColor(FLinearColor(0, 0, 1))
{
	LinkStyling.NormalPadding = FMargin(0);
	LinkStyling.PressedPadding = FMargin(0);
}

void ULinkRichTextBlock::OnLinkClicked_Implementation(FGameplayTag Tag)
{
	this->OnLinkClicked.Broadcast(Tag);
}

void ULinkRichTextBlock::GetLinkStyling_Implementation(FButtonStyle& OutStyle)
{
	OutStyle = this->LinkStyling;
}

FLinearColor ULinkRichTextBlock::GetLinkColor_Implementation()
{
	return this->LinkColor;
}

FLinearColor ULinkRichTextBlock::GetHoveredLinkColor_Implementation()
{
	return this->LinkColor;
}