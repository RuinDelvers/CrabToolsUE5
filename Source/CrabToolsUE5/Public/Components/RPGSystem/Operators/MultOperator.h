#pragma once

#include "Components/RPGSystem/Properties/IntAttribute.h"
#include "MultOperator.generated.h"

UCLASS(Blueprintable)
class UIntMultOperator : public UIntOperator {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ExposeOnSpawn=true))
	int Scalar;

public:
	virtual int Operate_Implementation(int Value) override;
};