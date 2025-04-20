#pragma once

#include "CoreMinimal.h"
#include "Components/UniformGridPanel.h"
#include "SortUniformGridPanel.generated.h"

/* Sortable uniform grid panel. Has a fixed width or height depending upon orientation. */
UCLASS()
class USortUniformGridPanel : public UUniformGridPanel
{
	GENERATED_BODY()	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Child Layout", meta=(AllowPrivateAccess=true, ClampMin=1, UIMin=1))
	int Width;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Child Layout", meta = (AllowPrivateAccess = true, ClampMin = 1, UIMin = 1))
	int Height;

public:

	UFUNCTION(BlueprintCallable, Category="Widget")
	void SortChildren();

};
