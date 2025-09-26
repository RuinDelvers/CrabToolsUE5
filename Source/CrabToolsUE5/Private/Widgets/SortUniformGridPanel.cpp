#include "Widgets/SortUniformGridPanel.h"
#include "Widgets/WidgetCompare.h"
#include "Components/UniformGridSlot.h"

void USortUniformGridPanel::SortChildren()
{
	this->Slots.Sort([&](const UPanelSlot& A, const UPanelSlot& B) {
		if (A.Content && A.Content->GetClass()->ImplementsInterface(UWidgetOrderInterface::StaticClass())) {
			return IWidgetOrderInterface::Execute_Compare(A.Content, B.Content);
		}
		else {
			return false;
		}
	});

	int index = 0;

	for (auto& SlotTemp : this->Slots) {
		auto GridSlot = Cast<UUniformGridSlot>(SlotTemp);
		int row = index / this->Width;
		int col = index % this->Width;

		GridSlot->SetRow(row);
		GridSlot->SetColumn(col);

		index += 1;
	}

	InvalidateLayoutAndVolatility();
}