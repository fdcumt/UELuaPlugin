#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "LuaWrapper.h"

class LUAWRAPPER_API FLuaWrapperModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	void Restart();
	void Init();

private:
	FLuaWrapper *m_LuaWrapper;
};