#include "Utils/WorldAwareObject.h"
#include "Utils/UtilsLibrary.h"

UWorld* UWorldAwareObject::GetWorld() const
{
	//Return null if the called from the CDO, or if the outer is being destroyed
	if (!HasAnyFlags(RF_ClassDefaultObject) && !GetOuter()->HasAnyFlags(RF_BeginDestroyed) && !GetOuter()->IsUnreachable())
	{
		//Try to get the world from the owning actor if we have one
		if (this->Owner)
		{
			return this->Owner->GetWorld();
		}
		else if (this->bAllowOuterOwner)
		{
			if (AActor* ActorOuter = UtilsFunctions::GetOuterAs<AActor>(this))
			{
				return this->Owner->GetWorld();
			}
		}
	}

	//Else return null - the latent action will fail to initialize
	return nullptr;
}