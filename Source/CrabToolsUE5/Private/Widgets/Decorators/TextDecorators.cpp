#include "Widgets/Decorators/TextDecorators.h"
#include "Components/RichTextBlock.h"


class FRichTextMapDecorator : public FRichTextDecorator
{
private:
	TObjectPtr<UTextMapDecorator> Mapper;

public:

	FRichTextMapDecorator(URichTextBlock* InOwner, UTextMapDecorator* InMapper)
		: FRichTextDecorator(InOwner), Mapper(InMapper)
	{
	}

protected:

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		if (this->Mapper->Text.Contains(RunParseResult.Name))
		{
			return true;
		}
		else
		{
			if (this->FindTableWithTerm(FName(RunParseResult.Name)))
			{
				return true;
			}
		}

		return false;
	}

	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const override
	{
		auto Key = FName(RunInfo.Name);
		auto Text = SNew(STextBlock);

		if (auto Table = this->FindTableWithTerm(Key))
		{
			auto Data = Table->FindRow<FTextMacroDataRow>(Key, "");

			Text->SetTextStyle(&Data->Style);
			Text->SetText(Data->DisplayText);
		}
		else
		{
			if (auto Data = this->Mapper->Text.Find(RunInfo.Name))
			{
				Text->SetTextStyle(&Data->Style);
				Text->SetText(Data->DisplayText);
			}			
		}

		//auto& Font = this->Owner->GetCurrentDefaultTextStyle().Font;
		//Text->SetFont(Font);
		Text->SetFont(TextStyle.Font);

		return Text;
	}
private:

	UDataTable* FindTableWithTerm(FName RowName) const
	{
		for (const auto& Table : this->Mapper->TextTables)
		{
			Table.LoadSynchronous();

			if (Table->FindRow<FTextMacroDataRow>(RowName, "", false))
			{
				return Table.Get();
			}
		}

		return nullptr;
	}
};

TSharedPtr<ITextDecorator> UTextMapDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShareable(new FRichTextMapDecorator(InOwner, this));
}