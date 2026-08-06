#pragma once

#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class IPropertyHandle;
class SMenuAnchor;
class SButton;
class SComboButton;

/**
 * 
 */
class CRABTOOLSUE5EDITOR_API SGenericPropertyPath : public SCompoundWidget
{

private:

	TSharedPtr<IPropertyHandle> PropertyHandle;
	TSharedPtr<SVerticalBox> PathContainer;
	TArray<FProperty*> Path;

public:

	SLATE_BEGIN_ARGS(SGenericPropertyPath)
		{
		}

	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> PropHandle);

	void Pop();
	void Reconstruct();
	void AddProperty(FProperty* NewProp);

private:

};
