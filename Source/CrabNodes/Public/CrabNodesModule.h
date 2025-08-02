#include "ICrabNodesModule.h"

class FStateMachineBlueprintCompiler;

class FCrabNodesModule: public ICrabNodesModule, public FGCObject
{

public:

	FCrabNodesModule();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;	
};