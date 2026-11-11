#include "StateMachine/EdGraph/StateConnectionDrawingPolicy.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/Widgets/Nodes/SEdEventEdge.h"

const float FStateMachineConnectionDrawingPolicy::LINE_SEPARATION_FACTOR = 8.5f;

FStateMachineConnectionDrawingPolicy::FStateMachineConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
	: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements)
	, GraphObj(InGraphObj)
{
}

void FStateMachineConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params)
{
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;
	Params.WireThickness = 1.5f;

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;
	if (bDeemphasizeUnhoveredPins)
	{
		ApplyHoverDeemphasis(OutputPin, InputPin, Params.WireThickness, Params.WireColor);
	}
}

void FStateMachineConnectionDrawingPolicy::Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries, FArrangedChildren& ArrangedNodes)
{
	// Build an acceleration structure to quickly find geometry for the nodes
	NodeWidgetMap.Empty();
	for (int32 NodeIndex = 0; NodeIndex < ArrangedNodes.Num(); ++NodeIndex)
	{
		FArrangedWidget& CurWidget = ArrangedNodes[NodeIndex];
		TSharedRef<SGraphNode> ChildNode = StaticCastSharedRef<SGraphNode>(CurWidget.Widget);
		NodeWidgetMap.Add(ChildNode->GetNodeObj(), NodeIndex);
	}

	// Now draw
	FConnectionDrawingPolicy::Draw(InPinGeometries, ArrangedNodes);
}

void FStateMachineConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint, const FVector2D& EndPoint, UEdGraphPin* Pin)
{
	FConnectionParams Params;
	DetermineWiringStyle(Pin, nullptr, /*inout*/ Params);

	if (Pin->Direction == EEdGraphPinDirection::EGPD_Output)
	{
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(PinGeometry, EndPoint), EndPoint, Params);
	}
	else
	{
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(PinGeometry, StartPoint), StartPoint, Params);
	}
}

void FStateMachineConnectionDrawingPolicy::DetermineLinkGeometry(
	FArrangedChildren& ArrangedNodes,
	TSharedRef<SWidget>& OutputPinWidget,
	UEdGraphPin* OutputPin,
	UEdGraphPin* InputPin,
	FArrangedWidget*& StartWidgetGeometry,
	FArrangedWidget*& EndWidgetGeometry
)
{
	if (UEdTransition* TransNode = Cast<UEdTransition>(InputPin->GetOwningNode()))
	{
		auto PrevState = TransNode->GetStartNode();
		auto NextState = TransNode->GetEndNode();
		if ((PrevState != NULL) && (NextState != NULL))
		{
			int32* PrevNodeIndex = NodeWidgetMap.Find(PrevState);
			int32* NextNodeIndex = NodeWidgetMap.Find(NextState);
			if ((PrevNodeIndex != NULL) && (NextNodeIndex != NULL))
			{
				StartWidgetGeometry = &(ArrangedNodes[*PrevNodeIndex]);
				EndWidgetGeometry = &(ArrangedNodes[*NextNodeIndex]);
			}
		}
	}
}

void FStateMachineConnectionDrawingPolicy::DrawSplineWithArrow(const FVector2D& StartAnchorPoint, const FVector2D& EndAnchorPoint, const FConnectionParams& Params)
{
	// bUserFlag1 indicates that we need to reverse the direction of connection (used by debugger)
	const FVector2D& P0 = Params.bUserFlag1 ? EndAnchorPoint : StartAnchorPoint;
	const FVector2D& P1 = Params.bUserFlag1 ? StartAnchorPoint : EndAnchorPoint;

	Internal_DrawLineWithArrow(P0, P1, Params);
}

void FStateMachineConnectionDrawingPolicy::Internal_DrawLineWithArrow(const FVector2D& StartAnchorPoint, const FVector2D& EndAnchorPoint, const FConnectionParams& Params)
{
	const SlateVector DeltaPos = SlateVector(EndAnchorPoint) - SlateVector(StartAnchorPoint);
	const SlateVector UnitDelta = DeltaPos.GetSafeNormal();
	const SlateVector Normal = SlateVector(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	// Come up with the final start/end points
	const SlateVector DirectionBias = Normal * LINE_SEPARATION_FACTOR;
	const SlateVector LengthBias = ArrowRadius.X * UnitDelta;
	const SlateVector StartPoint = SlateVector(StartAnchorPoint) + DirectionBias + LengthBias;
	const SlateVector EndPoint = SlateVector(EndAnchorPoint) + DirectionBias - LengthBias;
	FLinearColor ArrowHeadColor = Params.WireColor;

	FConnectionParams NewParams;

	// Need to recompute here, because ComputeSplineTangent is no longer called per update.
	NewParams.StartTangent = this->ComputeSplineTangent(StartAnchorPoint, EndAnchorPoint);
	NewParams.EndTangent = this->ComputeSplineTangent(StartAnchorPoint, EndAnchorPoint);

	// Draw a line/spline
	DrawConnection(WireLayerID, StartPoint, EndPoint, NewParams);
	// LengthBias * 0.8f is to ensure the line doesnt overlap the arrowhead glyph so they dont show up overlapping when semi-transparent
	//DrawConnection(WireLayerID, StartPoint, EndPoint - (LengthBias * 0.8f), Params);

	// Draw the arrow
	const SlateVector ArrowDrawPos = SlateVector(EndPoint) - SlateVector(ArrowRadius);
	const float AngleInRadians = FMath::Atan2(DeltaPos.Y, DeltaPos.X);
	
	this->Internal_DrawSplineHovered(StartPoint, EndPoint, LengthBias, ArrowHeadColor, Params);
	this->Internal_DrawRelinkTransitions(StartPoint, EndPoint, LengthBias, DeltaPos, Normal, ArrowHeadColor, Params);

	FSlateDrawElement::MakeRotatedBox(
		DrawElementsList,
		ArrowLayerID,
		FPaintGeometry(ArrowDrawPos, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
		ArrowImage,
		ESlateDrawEffect::None,
		AngleInRadians,
		TOptional<FVector2D>(),
		FSlateDrawElement::RelativeToElement,
		ArrowHeadColor
	);
}

void FStateMachineConnectionDrawingPolicy::Internal_DrawSplineHovered(const SlateVector& StartPoint, const SlateVector& EndPoint, const SlateVector& LengthBias, FLinearColor& ArrowHeadColor, const FConnectionParams& Params)
{
	// Draw the transition grab handles in case the mouse is hovering the transition
	bool bStartHovered = false;
	bool bEndHovered = false;
	const FVector2f FVecMousePos = FVector2f(LocalMousePosition.X, LocalMousePosition.Y);
	const FVector2f ClosestPoint = FMath::ClosestPointOnSegment2D(FVecMousePos, StartPoint, EndPoint);

	if ((ClosestPoint - FVecMousePos).Length() < RelinkHandleHoverRadius * ZoomFactor)
	{
		bStartHovered = (StartPoint - LocalMousePosition).Length() < RelinkHandleHoverRadius * ZoomFactor;
		bEndHovered = (EndPoint - LocalMousePosition).Length() < RelinkHandleHoverRadius * ZoomFactor;
		FVector2f HoverIndicatorPosition = bStartHovered ? StartPoint : EndPoint;

		// Set the hovered pin results. This will be used by the SGraphPanel again.
		const float SquaredDistToPin1 = (Params.AssociatedPin1 != nullptr) ? (StartPoint - LocalMousePosition).SizeSquared() : FLT_MAX;
		const float SquaredDistToPin2 = (Params.AssociatedPin2 != nullptr) ? (EndPoint - LocalMousePosition).SizeSquared() : FLT_MAX;
		UEdGraphPin* Pin1 = Params.AssociatedPin1;
		UEdGraphPin* Pin2 = Params.AssociatedPin2;
		if (Pin2)
		{
			// Forward transition links to the destination state node's output pin as only it has a valid pin widget
			if (UEdTransition* TransitionNode = Cast<UEdTransition>(Pin2->GetOwningNode()))
			{
				Pin2 = TransitionNode->GetEndNode()->GetInputPin();
			}
		}

		if (bStartHovered)
		{
			SplineOverlapResult = FGraphSplineOverlapResult(Pin2, Pin1, FMath::Min(SquaredDistToPin1, SquaredDistToPin2), SquaredDistToPin2, SquaredDistToPin1, true);
		}
		else if (bEndHovered)
		{
			SplineOverlapResult = FGraphSplineOverlapResult(Pin1, Pin2, FMath::Min(SquaredDistToPin1, SquaredDistToPin2), SquaredDistToPin1, SquaredDistToPin2, true);
		}

		// Draw grab handles only in case no relinking operation is performed
		if (RelinkConnections.IsEmpty() && !Params.bUserFlag2)
		{
			if (bStartHovered)
			{
				// Draw solid selection circle behind the arrow head in case the arrow head is hovered (area that enables a relink).
				static FSlateRoundedBoxBrush RoundedBoxBrush = FSlateRoundedBoxBrush(FStyleColors::Foreground, ArrowImage->ImageSize.X * 0.5f);

				FSlateDrawElement::MakeBox(DrawElementsList,
					ArrowLayerID - 1, // Draw behind the arrow
					FPaintGeometry(StartPoint - ArrowRadius + (LengthBias * 0.2f), ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
					&RoundedBoxBrush,
					ESlateDrawEffect::None,
					FStyleColors::AccentOrange.GetSpecifiedColor());
			}
			else if (bEndHovered)
			{
				// Draw solid selection circle behind the arrow head in case the arrow head is hovered (area that enables a relink).
				static FSlateRoundedBoxBrush RoundedBoxBrush = FSlateRoundedBoxBrush(FStyleColors::Foreground, ArrowImage->ImageSize.X * 0.5f);

				FSlateDrawElement::MakeBox(DrawElementsList,
					ArrowLayerID - 1, // Draw behind the arrow
					FPaintGeometry(EndPoint - ArrowRadius - (LengthBias * 0.2f), ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
					&RoundedBoxBrush,
					ESlateDrawEffect::None,
					FStyleColors::AccentOrange.GetSpecifiedColor());

				ArrowHeadColor = FLinearColor::Black;
			}
		}
	}
}

void FStateMachineConnectionDrawingPolicy::Internal_DrawRelinkTransitions(const SlateVector& StartPoint, const SlateVector& EndPoint, const SlateVector& LengthBias, const SlateVector& DeltaPos, const SlateVector& Normal, FLinearColor& ArrowHeadColor, const FConnectionParams& Params)
{
	// Draw the number of relinked transitions on the preview transition.
	if (!RelinkConnections.IsEmpty() && Params.bUserFlag2)
	{
		// Get the number of actually relinked transitions.
		int32 NumRelinkedTransitions = 0;
		for (const FRelinkConnection& Connection : RelinkConnections)
		{
			NumRelinkedTransitions += UEdTransition::GetListTransitionNodesToRelink(Connection.SourcePin, Connection.TargetPin, SelectedGraphNodes).Num();

			if (auto EntryNode = Cast<UEdStartStateNode>(Connection.SourcePin->GetOwningNode()))
			{
				NumRelinkedTransitions += 1;
			}
		}

		const FVector2f TransitionCenter = StartPoint + DeltaPos * 0.5f;
		const FVector2f TextPosition = TransitionCenter + Normal * 15.0f * ZoomFactor;

		FSlateDrawElement::MakeText(
			DrawElementsList,
			ArrowLayerID,
			FPaintGeometry(TextPosition, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
			FText::AsNumber(NumRelinkedTransitions),
			FCoreStyle::Get().GetFontStyle("SmallFont"));
	}
}

void FStateMachineConnectionDrawingPolicy::DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params)
{
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	DrawSplineWithArrow(StartAnchorPoint, EndAnchorPoint, Params);
}

FVector2D FStateMachineConnectionDrawingPolicy::ComputeSplineTangent(const FVector2D& Start, const FVector2D& End) const
{
	const FVector2D Delta = End - Start;
	const FVector2D NormDelta = Delta.GetSafeNormal();

	return NormDelta;
}
