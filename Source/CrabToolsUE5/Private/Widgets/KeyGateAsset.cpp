#include "Widgets/KeyGateAsset.h"

bool UKeyGateAsset::Route(const UKeyGateAsset* Gate, const FKey& Input)
{
	if (IsValid(Gate))
	{
		return Gate->ValidKeys.Contains(Input);
	}

	return false;
}