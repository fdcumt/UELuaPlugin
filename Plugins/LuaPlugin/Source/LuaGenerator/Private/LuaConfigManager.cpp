#include "LuaConfigManager.h"
#include "GeneratorDefine.h"

void FLuaConfigManager::Init()
{
	FString ConfigFilePath = NS_LuaGenerator::ProjectPath / NS_LuaGenerator::LuaConfigFileRelativePath;
	GConfig->GetArray(TEXT("SupportStructs"), TEXT("SupportStruct"), SupportStructs, ConfigFilePath);
	GConfig->GetArray(TEXT("AdditionalIncludeHeaders"), TEXT("IncludeHeader"), AdditionalIncludeHeaders, ConfigFilePath);
}
