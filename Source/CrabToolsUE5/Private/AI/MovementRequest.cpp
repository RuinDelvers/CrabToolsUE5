#include "AI/MovementRequest.h"
#include "NavigationPath.h"

FVector UAISimpleMovementRequest::GetEndLocation(UNavigationPath* Path)
{
    if (IsValid(Path))
    {
        if (Path->IsValid())
        {
            return Path->GetPath()->GetEndLocation();
        }
    }

    return FVector::ZeroVector;
}

FVector UAISimpleMovementRequest::GetStartLocation(UNavigationPath* Path)
{
    if (IsValid(Path))
    {
        if (Path->IsValid())
        {
            return Path->GetPath()->GetStartLocation();
        }
    }

    return FVector::ZeroVector;
}
