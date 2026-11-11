#pragma once

#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SNodePanel.h"
#include "SGraphNode.h"

class SToolTip;
class UEdTransition;

/*
 * Visual representation of an edge between two states. Since you can only transition from one state to another when
 * an event is sent, these are called Event edges.
 */
class SEdEventEdge : public SGraphNode
{

public:

	static FSlateColor StaticGetTransitionColor(UEdTransition* TransNode, bool bIsHovered);

public:
	SLATE_BEGIN_ARGS(SEdEventEdge){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdTransition* InNode);

	virtual bool RequiresSecondPassLayout() const override;
	virtual void PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const override;

	virtual void UpdateGraphNode() override;

	// Calculate position for multiple nodes to be placed between a start and end point, by providing this nodes index and max expected nodes 
	void PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const;

	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

protected:

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

	FSlateColor GetEdgeColor() const;

	const FSlateBrush* GetEdgeImage() const;

	EVisibility GetEdgeImageVisibility() const;
	EVisibility GetEdgeTitleVisbility() const;
private:
	TSharedPtr<STextEntryPopup> TextEntryWidget;
};