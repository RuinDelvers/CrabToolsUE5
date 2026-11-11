#include "StateMachine/Widgets/Nodes/SEdEventGraphPin.h"
#include "StateMachine/EdGraph/StateMachineGraphDragConnection.h"

void SEdEventGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != nullptr);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SEdEventGraphPin::GetPinBorder)
		.BorderBackgroundColor(this, &SEdEventGraphPin::GetPinColor)
		.OnMouseButtonDown(this, &SEdEventGraphPin::OnPinMouseDown)
		.Cursor(this, &SEdEventGraphPin::GetPinCursor)
		.Padding(FMargin(5.0f))
	);
}
