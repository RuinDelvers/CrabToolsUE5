#pragma once

#include "Components/RPGSystem/Properties/IntAttribute.h"
#include "AddOperator.generated.h"

UCLASS(Blueprintable)
class UIntAddOperator : public UIntOperator {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ExposeOnSpawn=true))
	int Additive;

public:
	virtual int Operate_Implementation(int Value) override;
};