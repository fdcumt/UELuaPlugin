#include "LuaConfigManager.h"
#include "GeneratorDefine.h"

void FLuaConfigManager::Init()
{
	ProjectPath = FPaths::GetPath(FPaths::GetProjectFilePath());
	FString ConfigFilePath = ProjectPath / NS_LuaGenerator::LuaConfigFileRelativePath;
	GConfig->GetArray(TEXT("SupportStructs"), TEXT("SupportStruct"), SupportStructs, ConfigFilePath);
	GConfig->GetArray(TEXT("AdditionalIncludeHeaders"), TEXT("IncludeHeader"), AdditionalIncludeHeaders, ConfigFilePath);
}
