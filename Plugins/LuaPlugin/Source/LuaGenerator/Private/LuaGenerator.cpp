#include "LuaGenerator.h"
#include "GeneratorDefine.h"
#include "ProjectDescriptor.h"
#include "Features/IModularFeatures.h"

#define LOCTEXT_NAMESPACE "FLuaPluginModule"

void FLuaGenerator::StartupModule()
{
	g_LuaConfigManager = new FLuaConfigManager();
	g_ScriptGeneratorManager = new FScriptGeneratorManager();
	g_LuaConfigManager->Init();
	IModularFeatures::Get().RegisterModularFeature(TEXT("ScriptGenerator"), this);
}

void FLuaGenerator::ShutdownModule()
{
	delete g_ScriptGeneratorManager;
	delete g_LuaConfigManager;
	IModularFeatures::Get().UnregisterModularFeature(TEXT("ScriptGenerator"), this);
}

FString FLuaGenerator::GetGeneratedCodeModuleName() const
{
	return NS_LuaGenerator::GameModuleName;
}

bool FLuaGenerator::ShouldExportClassesForModule(const FString& ModuleName, EBuildModuleType::Type ModuleType, const FString& ModuleGeneratedIncludeDirectory) const
{
	return m_SupportedModules.Contains(ModuleName);
}

bool FLuaGenerator::SupportsTarget(const FString& TargetName) const
{
	// if support the target, the project must have the config file(LuaConfig.ini), in config folder.
	if (FPaths::IsProjectFilePathSet())
	{
		FString ProjectFilePath = FPaths::GetProjectFilePath();
		FString projectPath = FPaths::GetPath(ProjectFilePath);
		FConfigFile* File = GConfig->Find(projectPath/NS_LuaGenerator::LuaConfigFileRelativePath, false);
		if (File != nullptr)
		{
			FProjectDescriptor ProjectDescriptor;
			FText OutError;

			// FPaths::GetProjectFilePath() return the  path of project's .uproject file
			ProjectDescriptor.Load(ProjectFilePath, OutError);

			// init the GameModuleName by the first module name in the file of ProjectName.uproject 
			ProjectDescriptor.Modules[0].Name.ToString(NS_LuaGenerator::GameModuleName); 
			return true;
		}
	}
	return false;
}

void FLuaGenerator::Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase)
{
	FString ConfigFilePath = g_LuaConfigManager->ProjectPath / NS_LuaGenerator::LuaConfigFileRelativePath;
	GConfig->GetArray(NS_LuaGenerator::SupportModuleSection, NS_LuaGenerator::SupportModuleKey, m_SupportedModules, ConfigFilePath);

	g_ScriptGeneratorManager->Initialize(RootLocalPath, RootBuildPath, OutputDirectory, IncludeBase);
}

void FLuaGenerator::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	g_ScriptGeneratorManager->ExportClass(Class, SourceHeaderFilename, GeneratedHeaderFilename, bHasChanged);
}

void FLuaGenerator::FinishExport()
{
	g_ScriptGeneratorManager->FinishExport();
}

FString FLuaGenerator::GetGeneratorName() const
{ // this function is for debug info
	return FString("LuaGenerator");
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLuaGenerator, LuaGenerator)