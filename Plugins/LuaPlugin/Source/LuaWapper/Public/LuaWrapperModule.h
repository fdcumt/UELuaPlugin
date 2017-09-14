#include "CoreMinimal.h"
#include "ModuleManager.h"

class FLuaWrapperModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};