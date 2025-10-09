#include "Widgets/Decorators/TagLinkDecorator.h"
#include "Widgets/LinkRichTextBlock.h"
#include "Logging/MessageLog.h"
#include "GameplayTagsManager.h"

#define LOCTEXT_NAMESPACE "TagLinkDecorator"


class SLinkTextBlock : public SButton
{

private:

	FGameplayTag LinkDestination;
	TObjectPtr<URichTextBlock> Owner;
	TSharedPtr<STextBlock> TextBlock;
	FButtonStyle Style;
	FLinearColor LinkColor;
	FLinearColor HoveredLinkColor;

public:
	SLATE_BEGIN_ARGS(SLinkTextBlock) {}
		SLATE_ARGUMENT(FTextBlockStyle, TextStyle)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, URichTextBlock* InOwner, FGameplayTag InTag, FText Content)
	{
		this->Owner = InOwner;
		this->LinkDestination = InTag;

		FSlateColor ButtonColor(FLinearColor(0, 0, 0, 0));

		ITagLinkListener::Execute_GetLinkStyling(this->Owner, this->Style);
		this->LinkColor = ITagLinkListener::Execute_GetLinkColor(this->Owner);
		this->HoveredLinkColor = ITagLinkListener::Execute_GetHoveredLinkColor(this->Owner);

		SButton::Construct(
			SButton::FArguments()
			.ButtonColorAndOpacity(ButtonColor)
			.ContentPadding(FMargin(0))
			.ButtonStyle(&this->Style)
			.ClickMethod(EButtonClickMethod::DownAndUp)
			.OnClicked_Raw(this, &SLinkTextBlock::OnClicked)
			.OnHovered_Raw(this, &SLinkTextBlock::OnHovered)
			.OnUnhovered(this, &SLinkTextBlock::OnUnhovered)
			[
				SAssignNew(TextBlock, STextBlock)
					.Text(Content)
					.TextStyle(&InArgs._TextStyle)
					.ColorAndOpacity(FSlateColor(this->LinkColor))
			]);
	}

	FReply OnClicked()
	{
		ITagLinkListener::Execute_OnLinkClicked(this->Owner, this->LinkDestination);
		return FReply::Handled();
	}

	void OnHovered()
	{	
		this->TextBlock->SetColorAndOpacity(this->HoveredLinkColor);
	}

	void OnUnhovered()
	{
		this->TextBlock->SetColorAndOpacity(this->LinkColor);
	}
};

class FTagLinkDecorator : public FRichTextDecorator
{
private:

	TObjectPtr<ULinkRichTextBlock> Root;


public:

	FTagLinkDecorator(URichTextBlock* InOwner)
		: FRichTextDecorator(InOwner), Root(CastChecked<ULinkRichTextBlock>(InOwner))
	{
	}

protected:

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		if (RunParseResult.Name == "link")
		{
			if (auto Data = RunParseResult.MetaData.Find("ref"))
			{
				return true;
			}
			else
			{
				#if WITH_EDITOR
					FMessageLog Log("LinkRichTextBlock");
					Log.Error(LOCTEXT("MissingRefError", "No tag given to link to."));
				#endif
				return false;
			}
		}

		return false;
	}

	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const override
	{		
		
		auto Tag = UGameplayTagsManager::Get().RequestGameplayTag(FName(*RunInfo.MetaData.Find("ref")), false);
		FText DisplayText = RunInfo.Content;

		#if WITH_EDITOR
			if (!Tag.IsValid())
			{
				FMessageLog Log("LinkRichTextBlock");
				Log.Error(LOCTEXT("MissingTagError", "Invalid tag given as a refernece."));
				DisplayText = LOCTEXT("InvalidTagError", "Error");
			}
		#endif

		auto Text = SNew(SLinkTextBlock, this->Root, Tag, DisplayText)
			.TextStyle(TextStyle);			

		return Text;
		//return TSharedPtr<SWidget>();
	}
};

TSharedPtr<ITextDecorator> UTagLinkDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShareable(new FTagLinkDecorator(InOwner));
}

#undef LOCTEXT_NAMESPACE