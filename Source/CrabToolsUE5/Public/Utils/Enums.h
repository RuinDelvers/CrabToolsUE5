#pragma once

#include "Enums.generated.h"

UENUM(BlueprintType)
enum class ESearchResult : uint8
{
	Found          UMETA(DisplayName = "Found"),
	NotFound       UMETA(DisplayName = "Missing"),
};

UENUM(BlueprintType)
enum class ENumericComparison : uint8
{
	EQUAL              UMETA(DisplayName = "Equals"),
	NOT_EQUAL          UMETA(DisplayName = "Not Equals"),
	LESS               UMETA(DisplayName = "Less"),
	LESS_OR_EQUAL      UMETA(DisplayName = "Less or Equals"),
	GREATER            UMETA(DisplayName = "Greater"),
	GREATER_OR_EQUAL   UMETA(DisplayName = "Greater or Equals"),
};

namespace NumericComparison
{
	template<class T>
	inline bool Compare(ENumericComparison Compare, T A, T B)
	{
		switch (Compare)
		{
			case ENumericComparison::EQUAL: return A == B;
			case ENumericComparison::NOT_EQUAL: return A != B;
			case ENumericComparison::LESS: return A < B;
			case ENumericComparison::LESS_OR_EQUAL: return A <= B;
			case ENumericComparison::GREATER: return A > B;
			case ENumericComparison::GREATER_OR_EQUAL: return A >= B;
			default: return false;
		}
	}
}