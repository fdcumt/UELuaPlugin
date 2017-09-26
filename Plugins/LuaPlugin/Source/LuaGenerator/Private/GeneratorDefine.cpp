#include "GeneratorDefine.h"

DEFINE_LOG_CATEGORY(LogLuaGenerator);

namespace NS_LuaGenerator
{
	FString ProjectPath;
	FString GameModuleName;
	FString ClassScriptHeaderSuffix(".script.h"); 

	const FString LuaConfigFileRelativePath("Config/LuaConfig.ini");

	const TCHAR* SupportModuleSection = TEXT("SupportModule");
	const TCHAR* SupportModuleKey = TEXT("SupportModuleKey");

	const TCHAR* NotSupportClassSection = TEXT("NotSupportClass");
	const TCHAR* NotSupportClassKey = TEXT("NotSupportClassKey");

	const TCHAR* ConfigClassFilesSection = TEXT("ConfigClassFiles");
	const TCHAR* ConfigClassFileKey = TEXT("ConfigClassFileName");
	const FString ClassConfigFileRelativeFolder("Config");

}