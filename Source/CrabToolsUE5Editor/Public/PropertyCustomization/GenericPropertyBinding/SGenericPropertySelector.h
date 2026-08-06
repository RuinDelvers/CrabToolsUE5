#pragma once

#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Properties/GenericPropertyBinding.h"
#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertyPath.h"
#include "PropertyCustomization/GenericPropertyBinding/SGenericPropertyField.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class IPropertyHandle;
class SMenuAnchor;
class SButton;
class SComboButton;
struct FPropertyChoice;

/**
 * 
 */
class CRABTOOLSUE5EDITOR_API SGenericPropertySelector : public SCompoundWidget
{

private:

	TSharedPtr<IPropertyHandle> PropertyHandle;
	TSharedPtr<SWidgetSwitcher> SelectorPages;
	TSharedPtr<SGenericPropertyPath> PathContainer;
	TSharedPtr<FGenericObjectPropertyBinding> Binding;

public:

	DECLARE_DELEGATE(FClosed);
	DECLARE_MULTICAST_DELEGATE(FClosedMulti);


	FClosedMulti OnClosed;

public:

	SLATE_BEGIN_ARGS(SGenericPropertySelector)
		{}

		SLATE_EVENT(FClosed, OnClosed)

	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> PropHandle);


private:

	bool IsConfirmable() const;
	void InitStructUnion();
	FReply OnCancelClicked() const;
	FReply OnConfirmClicked() const;
	FReply PopPath();
	bool CanNavigateBackward() const;
	void OnPropertyChosen_Inner(FPropertyChoice Property);
};

class CRABTOOLSUE5EDITOR_API SGenericPropertySelectorPage : public SCompoundWidget
{

private:

	TSharedPtr<IPropertyHandle> PropertyHandle;
	TSharedPtr<FGenericObjectPropertyBinding> Binding;

public:

	SGenericPropertyField::FSelectedMulti OnPropertyChosen;

public:

	SLATE_BEGIN_ARGS(SGenericPropertySelectorPage)
		{
		}
		SLATE_EVENT(SGenericPropertyField::FSelected, PropertyChosen)
	SLATE_END_ARGS();

	void Construct(
		const FArguments& InArgs,
		TSharedPtr<IPropertyHandle> PropHandle,
		FPropertyChoice SearchSource,
		TSharedPtr<FGenericObjectPropertyBinding> InitBinding);

private:

	/*
	 * Returns whether or not the given CheckProperty can be followed for achieving the final type required
	 * for GenericPropertyBinding found within PropHandle.
	 */
	bool ValidFieldForFinalType(
		TSharedPtr<IPropertyHandle> PropHandle, FProperty* CheckProperty);
};