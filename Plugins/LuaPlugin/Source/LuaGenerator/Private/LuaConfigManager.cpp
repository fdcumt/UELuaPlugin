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

	GConfig->GetArray(TEXT("BaseTypes"), TEXT("TypeName"), BaseTypes, ConfigFilePath);
	GConfig->GetArray(TEXT("SupportStructs"), TEXT("StructName"), SupportStructs, ConfigFilePath);
	GConfig->GetArray(TEXT("SupportModules"), TEXT("ModuleName"), SupportedModules, ConfigFilePath);
	GConfig->GetArray(TEXT("NotSupportClass"), TEXT("NotSupportClassKey"), NotSuportClasses, ConfigFilePath);
	GConfig->GetArray(TEXT("ConfigClassFiles"), TEXT("ConfigClassFileName"), ClassConfigFileNames, ConfigFilePath);
	GConfig->GetArray(TEXT("AdditionalIncludeHeaders"), TEXT("IncludeHeader"), AdditionalIncludeHeaders, ConfigFilePath);

	if (!FPaths::IsProjectFilePathSet())
	{
		DebugLog(TEXT("Project file path not set!"));
	}

	FConfigFile* File = GConfig->Find(ProjectPath/LuaConfigFileRelativePath, false);
	if (File == nullptr)
	{
		DebugLog(TEXT("not found config file %s!"), *(ProjectPath / LuaConfigFileRelativePath));
	}

	FText OutError;
	FProjectDescriptor ProjectDescriptor;

	// FPaths::GetProjectFilePath() return the  path of project's .uproject file
	ProjectDescriptor.Load(ProjectFilePath, OutError);

	// init the GameModuleName by the first module name in the file of ProjectName.uproject 
	ProjectDescriptor.Modules[0].Name.ToString(GameModuleName);
}
