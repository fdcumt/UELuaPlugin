#include "LuaConfigManager.h"
#include "GeneratorDefine.h"
#include "ProjectDescriptor.h"

void FLuaConfigManager::Init()
{
	ProjectPath.Empty();
	GameModuleName.Empty();
	ClassScriptHeaderSuffix = FString(".script.h");
	LuaConfigFileRelativePath = FString("Config/LuaConfig.ini");

	FString ProjectFilePath = FPaths::GetProjectFilePath();
	ProjectPath = FPaths::GetPath(ProjectFilePath);

	FString ConfigFilePath = ProjectPath / LuaConfigFileRelativePath;
	GConfig->GetArray(TEXT("SupportStructs"), TEXT("SupportStruct"), SupportStructs, ConfigFilePath);
	GConfig->GetArray(TEXT("AdditionalIncludeHeaders"), TEXT("IncludeHeader"), AdditionalIncludeHeaders, ConfigFilePath);

	if (FPaths::IsProjectFilePathSet())
	{
		FConfigFile* File = GConfig->Find(ProjectPath / LuaConfigFileRelativePath, false);
		if (File != nullptr)
		{
			FProjectDescriptor ProjectDescriptor;
			FText OutError;

			// FPaths::GetProjectFilePath() return the  path of project's .uproject file
			ProjectDescriptor.Load(ProjectFilePath, OutError);

			// init the GameModuleName by the first module name in the file of ProjectName.uproject 
			ProjectDescriptor.Modules[0].Name.ToString(GameModuleName);
		}
	}
}
