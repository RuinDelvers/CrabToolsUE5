#pragma once

#include "Components/RichTextBlockDecorator.h"
#include "Engine/DataTable.h"
#include "TextDecorators.generated.h"

USTRUCT(BlueprintType)
struct FTextMacroDataRow : public FTableRowBase
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FText DisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FTextBlockStyle Style;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	bool bOverrideFont = false;

	/* An icon to be rendered before the text. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FSlateBrush PreImage;

	/* An icon to be rendered after the text. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FSlateBrush PostImage;
};

/*
 * A rich text decorator that maps tags to a simple styled text. This is useful for
 * making simple text macros.
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UTextMapDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category="Style")
	TMap<FString, FTextMacroDataRow> Text;

	UPROPERTY(EditDefaultsOnly, Category = "Style",
		meta = (RequiredAssetDataTags = "RowStructure=/Script/CrabToolsUE5.TextMacroDataRow"))
	TArray<TSoftObjectPtr<UDataTable>> TextTables;

public:

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;
};