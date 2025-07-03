#include "StateMachine/StateMachine.h"
#include "Misc/CoreMiscDefines.h"
#include "InputDeviceNode.generated.h"

UCLASS(Blueprintable, Category = "StateMachine|Dialogue")
class CRABTOOLSUE5_API UInputDeviceNode : public UStateNode
{
	GENERATED_BODY()

protected:

	virtual void Initialize_Inner_Implementation() override;

	#if WITH_EDITOR
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif

private:

	UFUNCTION()
	void OnDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId);
};