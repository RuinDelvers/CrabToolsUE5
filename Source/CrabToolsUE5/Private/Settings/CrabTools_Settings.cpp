#include "Settings/CrabTools_Settings.h"
#include "Framework/Application/NavigationConfig.h"


/*
 * This will return the default keys used for UI navigation. This is based off the constructor in NavigationConfig.cpp
 * for the FNavigationConfig struct for Slate Applications.
 */
static TSet<FKey> UINavigationToDefaultKey(EUINavigation Nav)
{
	switch (Nav)
	{
		case EUINavigation::Left: return { EKeys::Left, EKeys::Gamepad_DPad_Left };
		case EUINavigation::Up: return { EKeys::Up, EKeys::Gamepad_DPad_Up };
		case EUINavigation::Right: return { EKeys::Right, EKeys::Gamepad_DPad_Right };
		case EUINavigation::Down: return { EKeys::Down, EKeys::Gamepad_DPad_Down };		
	}

	return {};
}

static TSet<FKey> UINavigationToDefaultKey(EUINavigationAction Nav)
{
	switch (Nav)
	{
		case EUINavigationAction::Accept: return {
			EKeys::Enter,
			EKeys::SpaceBar,
			EKeys::Virtual_Accept,
		};
		case EUINavigationAction::Back: return {
			EKeys::Escape,
			EKeys::Virtual_Back,
		};
	}

	return {};
}

UCrabTools_Settings::UCrabTools_Settings()
{

	for (auto Nav : TEnumRange<EUINavigation>())
	{
		for (auto& Key : UINavigationToDefaultKey(Nav))
		{
			this->NavControl.Add(Key, Nav);
		}
		
	}

	for (auto Action : TEnumRange<EUINavigationAction>())
	{
		for (auto& Key : UINavigationToDefaultKey(Action))
		{
			this->NavActions.Add(Key, Action);
		}
	}
}

const UCrabTools_Settings* UCrabTools_Settings::GetSettings()
{
	return GetDefault<UCrabTools_Settings>();
}

void UCrabTools_Settings::ApplyCustomUINavigation() const
{
	FNavigationConfig Config;

	Config.KeyEventRules.Empty();
	Config.KeyActionRules.Empty();

	for (const auto& NavRules : this->NavControl)
	{
		Config.KeyEventRules.Add(NavRules.Key, NavRules.Value);
	}

	for (const auto& Action : this->NavActions)
	{
		Config.KeyActionRules.Add(Action.Key, Action.Value);
	}

	FSlateApplication::Get().SetNavigationConfig(MakeShared<FNavigationConfig>(Config));
}

void UCrabTools_Settings::PostInitProperties()
{
	Super::PostInitProperties();
	
	// Bind callbacks here.
}