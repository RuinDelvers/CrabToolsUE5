#pragma once

#include "Components/RichTextBlockDecorator.h"
#include "Widgets/LinkRichTextBlock.h"
#include "TagLinkDecorator.generated.h"

/*
 * A rich text decorator that maps tags to a simple styled text. This is useful for
 * making simple text macros.
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UTagLinkDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;
};