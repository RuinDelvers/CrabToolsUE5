#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PathFindingUtils.generated.h"

class AAIController;
class ANavLinkProxy;

/**
 * 
 */
UCLASS()
class UPathFindingUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="Utility")
	static FVector ChooseNearLocation(AAIController* Ctrl, TArray<FVector>& Points);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static FVector NearestNavLinkPointToActor(ANavLinkProxy* NLP, AActor* Actor);
};
