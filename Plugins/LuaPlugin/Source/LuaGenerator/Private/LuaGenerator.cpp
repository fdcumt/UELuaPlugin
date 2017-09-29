#include "LuaGenerator.h"
#include "GeneratorDefine.h"
#include "ProjectDescriptor.h"
#include "Features/IModularFeatures.h"

#define LOCTEXT_NAMESPACE "FLuaPluginModule"

void FLuaGenerator::StartupModule()
{
	m_LuaScriptGenerator = new FScriptGeneratorManager();
	g_ScriptGeneratorManager = m_LuaScriptGenerator;
	IModularFeatures::Get().RegisterModularFeature(TEXT("ScriptGenerator"), this);
}

void FLuaGenerator::ShutdownModule()
{
	delete m_LuaScriptGenerator;
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
		NS_LuaGenerator::ProjectPath = FPaths::GetPath(ProjectFilePath);
		FConfigFile* File = GConfig->Find(NS_LuaGenerator::ProjectPath/NS_LuaGenerator::LuaConfigFileRelativePath, false);
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
	FString ConfigFilePath = NS_LuaGenerator::ProjectPath / NS_LuaGenerator::LuaConfigFileRelativePath;
	GConfig->GetArray(NS_LuaGenerator::SupportModuleSection, NS_LuaGenerator::SupportModuleKey, m_SupportedModules, ConfigFilePath);

	m_LuaScriptGenerator->Initialize(RootLocalPath, RootBuildPath, OutputDirectory, IncludeBase);
}

void FLuaGenerator::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	m_LuaScriptGenerator->ExportClass(Class, SourceHeaderFilename, GeneratedHeaderFilename, bHasChanged);
}

void FLuaGenerator::FinishExport()
{
	m_LuaScriptGenerator->FinishExport();
}

FString FLuaGenerator::GetGeneratorName() const
{ // this function is for debug info
	return FString("LuaGenerator");
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLuaGenerator, LuaGenerator)