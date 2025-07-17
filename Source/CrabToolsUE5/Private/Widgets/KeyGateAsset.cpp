#include "Widgets/KeyGateAsset.h"

bool UKeyGateAsset::Route(const UKeyGateAsset* Gate, const FKey& Input)
{
	return Gate->ValidKeys.Contains(Input);
}