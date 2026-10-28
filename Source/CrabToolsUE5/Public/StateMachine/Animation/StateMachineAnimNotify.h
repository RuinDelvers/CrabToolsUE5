#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "StateMachine/StateMachine.h"
#include "Misc/EngineVersionComparison.h"
#include "StateMachineAnimNotify.generated.h"

class USkeletalMeshComponent;
class UAnimSequenceBase;

/* Anim Notify used to send a specified event to an EventListener interface*/
UCLASS()
class UStateMachineAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine")
	FName EventToEmit;

	#if UE_VERSION_OLDER_THAN(5, 8, 0)
		virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override {}
	#else
		virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override {}
	#endif
};