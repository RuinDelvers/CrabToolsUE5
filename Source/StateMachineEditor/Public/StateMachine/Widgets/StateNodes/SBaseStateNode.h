#pragma once

#include "Widgets/SCompoundWidget.h"

class UStateNode;
class UEdStateNode;

class STATEMACHINEEDITOR_API SBaseStateNodeDetails : public SCompoundWidget, public FNotifyHook
{

protected:

	TWeakObjectPtr<UStateNode> StateNode;
	TWeakObjectPtr<UEdStateNode> EdStateNode;
	TSharedPtr<IDetailsView> Inspector;

public:

	SLATE_BEGIN_ARGS(SBaseStateNodeDetails) {}
	SLATE_END_ARGS()

	virtual ~SBaseStateNodeDetails();

	void Construct(const FArguments& InArgs, UEdStateNode* OwningState, UStateNode* InNode);

	TSharedPtr<IDetailsView> GetInspector();

private:

	FReply OnDeleteClicked();
	FReply OnExpandClicked();

	const FSlateBrush* GetExpanderBrush() const;

	FReply OnHeaderClicked(const FGeometry& Geometry, const FPointerEvent& Event);
};