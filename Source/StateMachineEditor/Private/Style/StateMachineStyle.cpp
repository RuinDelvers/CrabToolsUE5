#include "Style/StateMachineStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateTypes.h"
#include "Styling/SlateStyle.h"
#include "Styling/AppStyle.h"
#include "Styling/StyleColors.h"
#include "StateMachine/StateMachine.h"
#include "Components/StateMachineComponent.h"

class FStateMachineStyleSet final : public FSlateStyleSet
{
public:

	FStateMachineStyleSet() : FSlateStyleSet(FStateMachineStyle::GetStyleSetName())
	{
		const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("CrabToolsUE5"));

		FString StyleRoot = Plugin->GetBaseDir();
		StyleRoot = FPaths::Combine(StyleRoot, TEXT("Resources"), TEXT("Editor"));

		this->SetContentRoot(StyleRoot);

		this->AddClassIcon<UStateMachine>();
		this->AddClassIcon<UStateMachineComponent>();

		
	}

private:

	template <class T>
	void AddClassIcon()
	{
		const FString CFCIconPropName = FString::Printf(TEXT("ClassIcon.%s"), *T::StaticClass()->GetName());
		const FString CFCIconPath = FString::Printf(TEXT("/Icons/%sClassIcon"), *T::StaticClass()->GetName());
		FSlateImageBrush* CFCIconBrush = new FSlateImageBrush(this->RootToContentDir(CFCIconPath, TEXT(".png")), FVector2D(20.f, 20.f));

		this->Set(*CFCIconPropName, CFCIconBrush);
	}
};


TSharedPtr<FSlateStyleSet> FStateMachineStyle::StyleSet = nullptr;


void FStateMachineStyle::Initialize()
{
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShared<FStateMachineStyleSet>();

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

void FStateMachineStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}

FName FStateMachineStyle::GetStyleSetName()
{
	static FName StyleName("StateMachineStyle");
	return StyleName;
}
