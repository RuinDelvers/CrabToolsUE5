﻿// Copyright Epic Games, Inc. All Rights Reserved.

#include "PropertyCustomization/EventSlot/SEventSlotChip.h"
#include "PropertyCustomization/EventSlot/SEventSlotPicker.h"
#include "DetailLayoutBuilder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Styling/StyleColors.h"
#include "Styles/EventSlotStyle.h"
#include "Framework/Application/SlateApplication.h"

#define LOCTEXT_NAMESPACE "EventSlotChip"

SLATE_IMPLEMENT_WIDGET(SEventSlotChip)
void SEventSlotChip::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "IsSelected", IsSelectedAttribute, EInvalidateWidgetReason::Layout)
		.OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda([](SWidget& Widget)
			{
				static_cast<SEventSlotChip&>(Widget).UpdatePillStyle();
			}));

	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "ShowClearButton", ShowClearButtonAttribute, EInvalidateWidgetReason::Layout);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "Text", TextAttribute, EInvalidateWidgetReason::Layout);
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "ToolTipText", ToolTipTextAttribute, EInvalidateWidgetReason::Layout);
}

SEventSlotChip::SEventSlotChip()
	: IsSelectedAttribute(*this)
	, ShowClearButtonAttribute(*this)
	, ToolTipTextAttribute(*this)
	, TextAttribute(*this)
{
}

void SEventSlotChip::Construct(const FArguments& InArgs)
{
	IsSelectedAttribute.Assign(*this, InArgs._IsSelected);
	ShowClearButtonAttribute.Assign(*this, InArgs._ShowClearButton);
	ToolTipTextAttribute.Assign(*this, InArgs._ToolTipText);
	TextAttribute.Assign(*this, InArgs._Text);
	OnClearPressed = InArgs._OnClearPressed;
	OnEditPressed = InArgs._OnEditPressed;
	OnNavigate = InArgs._OnNavigate;
	OnMenu = InArgs._OnMenu;
	bEnableNavigation = InArgs._EnableNavigation;

	TWeakPtr<SEventSlotChip> WeakSelf = StaticCastWeakPtr<SEventSlotChip>(AsWeak());

	ChildSlot
	[
		SNew(SBox)
		.HeightOverride(18.0f)
		[
			SNew(SBorder)
			.OnMouseButtonDown_Lambda([WeakSelf](const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
			{
				const TSharedPtr<SEventSlotChip> Self = WeakSelf.Pin();
				if (Self.IsValid() && MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
				{
					if (Self->OnMenu.IsBound())
					{
						return Self->OnMenu.Execute(MouseEvent);
					}
				}
				return FReply::Unhandled();
			})
			.Padding(0.0f)
			.BorderImage(FStyleDefaults::GetNoBrush())
			[
				SNew(SHorizontalBox)
				
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Fill)
				.AutoWidth()
				[
					SAssignNew(ChipButton, SButton)
					.ContentPadding(0)
					.ButtonStyle(FEventSlotStyle::Get(), "EventSlot.ChipButton.Selected")
					.ToolTipText_Lambda([WeakSelf]()
					{
						const TSharedPtr<SEventSlotChip> Self = WeakSelf.Pin();
						return Self.IsValid() ? Self->ToolTipTextAttribute.Get() : FText::GetEmpty();
					})
					.IsEnabled(!InArgs._ReadOnly)
					.OnClicked_Lambda([WeakSelf]()
					{
						if (const TSharedPtr<SEventSlotChip> Self = WeakSelf.Pin())
						{
							const FModifierKeysState ModifierKeys = FSlateApplication::Get().GetModifierKeys();
							if (Self->bEnableNavigation && ModifierKeys.IsControlDown())
							{
								if (Self->OnNavigate.IsBound())
								{
									return Self->OnNavigate.Execute();
								}
							}
								
							if (Self->OnEditPressed.IsBound())
							{
								return Self->OnEditPressed.Execute();
							}
						}							
						return FReply::Unhandled();
					})
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Font(FAppStyle::GetFontStyle( TEXT("PropertyWindow.NormalFont")))
							.Text_Lambda([WeakSelf]()
							{
								const TSharedPtr<SEventSlotChip> Self = WeakSelf.Pin();
								return Self.IsValid() ? Self->TextAttribute.Get() : FText::GetEmpty();
							})
							.StrikeBrush_Lambda([WeakSelf]()
							{
								if (const TSharedPtr<SEventSlotChip> Self = WeakSelf.Pin())
								{
									const FModifierKeysState ModifierKeys = FSlateApplication::Get().GetModifierKeys();
									if (Self->bEnableNavigation && ModifierKeys.IsControlDown())
									{
										return FEventSlotStyle::Get().GetBrush("EventSlot.Chip.Underline");
									}
								}
								return FAppStyle::Get().GetBrush("NoBrush");
							})
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						.Padding(0)
						[
							SAssignNew(ClearButton, SButton)
							.IsEnabled(!InArgs._ReadOnly)
							.Visibility_Lambda([WeakSelf]()
							{
								const TSharedPtr<SEventSlotChip> Self = WeakSelf.Pin();
								return (Self.IsValid() && Self->ShowClearButtonAttribute.Get()) ? EVisibility::Visible : EVisibility::Collapsed;
							})
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Center)
							.ButtonStyle(FEventSlotStyle::Get(), "EventSlot.ChipClearButton")
							.ToolTipText(LOCTEXT("ClearTag", "Clear Tag"))
							.ContentPadding(FMargin(2, 0, 0, 0))
							.OnClicked_Lambda([WeakSelf]()
							{
								const TSharedPtr<SEventSlotChip> Self = WeakSelf.Pin();
								if (Self.IsValid() && Self->OnClearPressed.IsBound())
								{
									return Self->OnClearPressed.Execute();
								}
								return FReply::Unhandled();
							})
							[
								SNew(SImage)
								.ColorAndOpacity_Lambda([WeakSelf]()
								{
									const TSharedPtr<SEventSlotChip> Self = WeakSelf.Pin();
									if (Self.IsValid() && Self->ClearButton.IsValid())
									{
										return Self->ClearButton->IsHovered() ? FStyleColors::White : FStyleColors::Foreground;
									}
									return FStyleColors::Foreground;
								})
								.Image(FAppStyle::GetBrush("Icons.X"))
								.DesiredSizeOverride(FVector2D(12.0f, 12.0f))
							]
						]
					]
				]
			]
		]
	];

	bLastHasIsSelected = true;
	UpdatePillStyle();
}

void SEventSlotChip::UpdatePillStyle()
{
	const bool bIsSelected = IsSelectedAttribute.Get();

	if (bIsSelected != bLastHasIsSelected)
	{
		if (bIsSelected)
		{
			ChipButton->SetButtonStyle(&FEventSlotStyle::Get().GetWidgetStyle<FButtonStyle>("EventSlot.ChipButton.Selected"));
		}
		else
		{
			ChipButton->SetButtonStyle(&FEventSlotStyle::Get().GetWidgetStyle<FButtonStyle>("EventSlot.ChipButton.Unselected"));
		}
		bLastHasIsSelected = bIsSelected;
	}
}

#undef LOCTEXT_NAMESPACE
