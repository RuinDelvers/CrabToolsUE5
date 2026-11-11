#pragma once

#include "CoreMinimal.h"
#include "ConnectionDrawingPolicy.h"

// This is used since slate eventually stopped support double vectors, and only using floats.
#if ENGINE_MINOR_VERSION >= 7
	using SlateVector = FVector2f;
#else
	using SlateVector = FVector2D;
#endif

class FStateMachineConnectionDrawingPolicy : public FConnectionDrawingPolicy
{

public:

	static constexpr float RelinkHandleHoverRadius = 10.0f;
	static const float LINE_SEPARATION_FACTOR;

protected:
	UEdGraph* GraphObj;
	TMap<UEdGraphNode*, int32> NodeWidgetMap;

public:
	FStateMachineConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj);

	// FConnectionDrawingPolicy interface 
	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params) override;
	virtual void Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& PinGeometries, FArrangedChildren& ArrangedNodes) override;
	virtual void DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params) override;
	virtual void DrawSplineWithArrow(const FVector2D& StartPoint, const FVector2D& EndPoint, const FConnectionParams& Params) override;
	virtual void DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint, const FVector2D& EndPoint, UEdGraphPin* Pin) override;
	virtual FVector2D ComputeSplineTangent(const FVector2D& Start, const FVector2D& End) const override;
	virtual void DetermineLinkGeometry(FArrangedChildren& ArrangedNodes, TSharedRef<SWidget>& OutputPinWidget, UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FArrangedWidget*& StartWidgetGeometry, FArrangedWidget*& EndWidgetGeometry) override;
	// End of FConnectionDrawingPolicy interface



protected:
	void Internal_DrawLineWithArrow(
		const FVector2D& StartAnchorPoint,
		const FVector2D& EndAnchorPoint,
		const FConnectionParams& Params);

	void Internal_DrawSplineHovered(
		const SlateVector& StartPoint,
		const SlateVector& EndPoint,
		const SlateVector& LengthBias,
		FLinearColor& ArrowHeadColor,
		const FConnectionParams& Params);

	void Internal_DrawRelinkTransitions(
		const SlateVector& StartPoint,
		const SlateVector& EndPoint,
		const SlateVector& LengthBias,
		const SlateVector& DeltaPos,
		const SlateVector& Normal,
		FLinearColor& ArrowHeadColor,
		const FConnectionParams& Params);
};
