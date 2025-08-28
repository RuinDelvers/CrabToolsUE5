#pragma once

#include "Components/RPGSystem/Properties/IntAttribute.h"
#include "FunctionOperator.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(int, FIntOpFunction, int, Value);

UCLASS(Blueprintable)
class UIntFunctionOperator : public UIntOperator {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, GetOptions="GetIntOperatorFunctions"))
	FName FunctionName;
	FIntOpFunction Callback;

public:
	virtual int Operate_Implementation(int Value) override;
	virtual void Initialize_Implementation() override;

	UFUNCTION()
	TArray<FString> GetIntOperatorFunctions() const;
};