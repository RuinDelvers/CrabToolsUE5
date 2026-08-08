#pragma once

#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class IPropertyHandle;
class SMenuAnchor;
class SButton;
class SComboButton;
struct FGenericObjectPropertyBinding;

struct FPropertyChoice
{
	FProperty* Property = nullptr;
	TWeakObjectPtr<UFunction> Function;
	const UClass* CastClass = nullptr;


	FPropertyChoice() {}
	FPropertyChoice(FProperty* InitProperty, UFunction* InitFunction = nullptr, const UClass* InitCastClass = nullptr)
		: Property(InitProperty), Function(InitFunction), CastClass(InitCastClass) {}

	/*
	 * Returns the class to be searched for properties, which includes checking for the forwarded class to be
	 */
	const UStruct* GetSearchStruct(TSharedPtr<IPropertyHandle> Default) const;

	operator bool() const { return Property != nullptr; }

	void ApplyToBinding(TSharedPtr<FGenericObjectPropertyBinding>& Binding) const;

	bool IsCastableValue() const;
};

/**
 * 
 */
class CRABTOOLSUE5EDITOR_API SGenericPropertyField : public SCompoundWidget
{

public:

	DECLARE_DELEGATE_OneParam(FSelected, FPropertyChoice);
	DECLARE_MULTICAST_DELEGATE_OneParam(FSelectedMulti, FPropertyChoice);

private:

	TSharedPtr<IPropertyHandle> PropertyHandle;
	TSharedPtr<SBorder> HighlightBorder;
	FPropertyChoice Choice;
	FSelectedMulti OnSelect;
	bool bMouseDown = false;
	TAttribute<bool> bSelectable;

public:

	SLATE_BEGIN_ARGS(SGenericPropertyField)
		{
		}
		SLATE_EVENT(FSelected, OnSelect)
		SLATE_ATTRIBUTE(bool, Selectable)
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs, TSharedPtr<IPropertyHandle> PropHandle, FPropertyChoice Inithoice);

	virtual FReply OnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& Geometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

private:

	UClass* GetResultClass() const;
	FReply OnCastClassClicked();
	TSharedRef<STextBlock> GetLabel() const;	
	void OnClassChosen(const UClass* Chosen);
};
