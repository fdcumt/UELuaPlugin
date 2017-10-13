#include "LuaConfigManager.h"
#include "GeneratorDefine.h"
#include "ProjectDescriptor.h"

void FLuaConfigManager::Init()
{
	ProjectPath.Empty();
	GameModuleName.Empty();

	ClassScriptHeaderSuffix = ".script.h";
	ClassConfigFileRelativeFolder = "Config";

	FString ProjectFilePath = FPaths::GetProjectFilePath();
	ProjectPath = FPaths::GetPath(ProjectFilePath);
	LuaConfigFileRelativePath = FString("Config/LuaConfig.ini");
	FString ConfigFilePath = ProjectPath / LuaConfigFileRelativePath;

	GConfig->GetArray(TEXT("BaseType"), TEXT("BaseTypeKey"), BaseTypes, ConfigFilePath);
	GConfig->GetArray(TEXT("SupportStructs"), TEXT("SupportStruct"), SupportStructs, ConfigFilePath);
	GConfig->GetArray(TEXT("SupportModule"), TEXT("SupportModuleKey"), SupportedModules, ConfigFilePath);
	GConfig->GetArray(TEXT("NotSupportClass"), TEXT("NotSupportClassKey"), NotSuportClasses, ConfigFilePath);
	GConfig->GetArray(TEXT("ConfigClassFiles"), TEXT("ConfigClassFileName"), ClassConfigFileNames, ConfigFilePath);
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
