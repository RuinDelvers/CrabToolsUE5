#include "StateMachine/Widgets/StateNodes/SBaseStateNode.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/Widgets/Utils.h"

#define LOCTEXT_NAMESPACE "SBaseStateNode"

SBaseStateNodeDetails::~SBaseStateNodeDetails()
{
}

void SBaseStateNodeDetails::Construct(const FArguments& InArgs, UEdStateNode* OwningState, UStateNode* InNode)
{
	this->StateNode = InNode;
	this->EdStateNode = OwningState;

	auto DetailsViewer = this->GetInspector().ToSharedRef();

	this->ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryTop"))
				.OnMouseButtonUp(this, &SBaseStateNodeDetails::OnHeaderClicked)
				[
					
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Left)
					[
						SNew(SButton)
							.OnClicked(this, &SBaseStateNodeDetails::OnExpandClicked)
							.ButtonStyle(FAppStyle::Get(), "SimpleButton")
							.Text(FText::FromString("Expand"))
							.ContentPadding(FMargin(1, 0))
							[
								SNew(SImage)
									.Image(this, &SBaseStateNodeDetails::GetExpanderBrush)
									.ColorAndOpacity(FSlateColor::UseForeground())
							]
					]
					+SHorizontalBox::Slot()
					.HAlign(HAlign_Fill)
						[
							SNew(STextBlock)
								.Text(FText::FromString(InNode->GetClass()->GetName()))
								.Font(FAppStyle::GetFontStyle("DetailsView.CategoryTextStyle"))
						]
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Right)
						[
							SNew(SButton)
								.OnClicked(this, &SBaseStateNodeDetails::OnDeleteClicked)
								.ButtonStyle(FAppStyle::Get(), "SimpleButton")
								.Text(FText::FromString("Delete"))
								.ContentPadding(FMargin(1, 0))
								[
									SNew(SImage)
										.Image(FAppStyle::Get().GetBrush("Icons.Denied"))
										.ColorAndOpacity(FSlateColor::UseForeground())
								]
						]
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Fill)
			[
				DetailsViewer
			]
		];

	DetailsViewer->SetObject(InNode);
}

TSharedPtr<IDetailsView> SBaseStateNodeDetails::GetInspector()
{
	if (!this->Inspector.IsValid())
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

		FDetailsViewArgs DetailsViewArgs;
		DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
		DetailsViewArgs.bUpdatesFromSelection = false;
		DetailsViewArgs.bLockable = false;
		DetailsViewArgs.bShowPropertyMatrixButton = false;
		DetailsViewArgs.NotifyHook = this;

		DetailsViewArgs.ViewIdentifier = "Machine Details";
		DetailsViewArgs.bAllowSearch = false;
		DetailsViewArgs.bAllowFavoriteSystem = false;
		DetailsViewArgs.bShowOptions = false;
		DetailsViewArgs.bShowModifiedPropertiesOption = true;
		DetailsViewArgs.bShowKeyablePropertiesOption = true;
		DetailsViewArgs.bShowAnimatedPropertiesOption = false;
		DetailsViewArgs.bShowScrollBar = true;
		DetailsViewArgs.bForceHiddenPropertyVisibility = false;
		DetailsViewArgs.ColumnWidth = 0.3f;
		DetailsViewArgs.bShowCustomFilterOption = true;

		Inspector = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

		Inspector->SetCustomFilterLabel(LOCTEXT("ShowAllParameters", "Show All Parameters"));
		Inspector->SetIsPropertyVisibleDelegate(
			FIsPropertyVisible::CreateStatic(
				&StateMachineEditor::Widgets::DetailWidgetPropertyFilter));

		Inspector->SetVisibility(EVisibility::Collapsed);
	}

	return this->Inspector;
}

FReply SBaseStateNodeDetails::OnDeleteClicked()
{
	this->EdStateNode->RemoveStateNode(this->StateNode.Get());
	return FReply::Handled();
}

FReply SBaseStateNodeDetails::OnExpandClicked()
{
	if (this->Inspector->GetVisibility() == EVisibility::Collapsed)
	{
		this->Inspector->SetVisibility(EVisibility::Visible);
	}
	else
	{
		this->Inspector->SetVisibility(EVisibility::Collapsed);
	}
	
	return FReply::Handled();
}

const FSlateBrush* SBaseStateNodeDetails::GetExpanderBrush() const
{
	if (this->Inspector->GetVisibility() == EVisibility::Collapsed)
	{
		return FAppStyle::Get().GetBrush("Symbols.RightArrow");
	}
	else
	{
		return FAppStyle::Get().GetBrush("Symbols.DownArrow");
	}	
}

FReply SBaseStateNodeDetails::OnHeaderClicked(const FGeometry& Geometry, const FPointerEvent& Event)
{
	if (Event.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		this->OnExpandClicked();
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

