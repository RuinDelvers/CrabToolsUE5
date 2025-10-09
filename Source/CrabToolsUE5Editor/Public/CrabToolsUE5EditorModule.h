#include "ICrabToolsUE5EditorModule.h"
#include "Editor/UnrealEdEngine.h"

class FCrabToolsUE5EditorModule: public ICrabToolsUE5EditorModule, public FGCObject
{
private:

	TSet<TObjectPtr<class APatrolPath>> OldSelectedPatrolPaths;
	TSet<TObjectPtr<class ADynamicActorSpawner>> OldSelectedSpawners;

	FDelegateHandle SelectionChangedHandle;
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;

public:

	FCrabToolsUE5EditorModule();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; }
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return ToolBarExtensibilityManager; }
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override {}
	virtual FString GetReferencerName() const override;

private:

	void OnSelectionChanged(UObject* Obj);
	void OnPostEngineInit();
	void InitializeCustomization();
};