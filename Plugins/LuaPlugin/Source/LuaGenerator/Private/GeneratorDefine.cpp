#include "GeneratorDefine.h"

DEFINE_LOG_CATEGORY(LogLuaGenerator);

namespace NS_LuaGenerator
{
	FString ProjectPath;
	FString GameModuleName;
	const TCHAR* LuaConfigSectionName = TEXT("Lua");
	const TCHAR* LuaConfigKeyName = TEXT("SupportModule");
	const FString LuaConfigFileRelativePath("Config/LuaConfig.ini");

}