#pragma once

#include "EdGraphUtilities.h"
#include "Templates/SharedPointer.h"

class SGraphPin;

class CRABNODES_API FStateMachineGraphPanelPinFactory: public FGraphPanelPinFactory
{
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override;
};
