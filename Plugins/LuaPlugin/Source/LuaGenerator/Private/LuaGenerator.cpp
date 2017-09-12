#include "LuaGenerator.h"
#include "ProjectDescriptor.h"
#include "Features/IModularFeatures.h"

DEFINE_LOG_CATEGORY(LogLuaGenerator);

#define LOCTEXT_NAMESPACE "FLuaPluginModule"

void FLuaGenerator::StartupModule()
{
	IModularFeatures::Get().RegisterModularFeature(TEXT("ScriptGenerator"), this);
}

void FLuaGenerator::ShutdownModule()
{
	IModularFeatures::Get().UnregisterModularFeature(TEXT("ScriptGenerator"), this);
}

FString FLuaGenerator::GetGeneratedCodeModuleName() const
{
	UE_LOG(LogLuaGenerator, Warning, TEXT("GetGeneratedCodeModuleName"));
	return FString("MyProject");
}

bool FLuaGenerator::ShouldExportClassesForModule(const FString& ModuleName, EBuildModuleType::Type ModuleType, const FString& ModuleGeneratedIncludeDirectory) const
{
	return true;
}


bool FLuaGenerator::SupportsTarget(const FString& TargetName) const
{
	// if support the target, the project must have the config file(LuaConfig.ini), in config folder.
	if (FPaths::IsProjectFilePathSet())
	{
		FString ProjectFilePath = FPaths::GetProjectFilePath();
		FString ProjectPath = FPaths::GetPath(ProjectFilePath);
		FConfigFile* File = GConfig->Find(ProjectPath /"Config/LuaConfig.ini", false);
		if (File != nullptr)
		{
			FProjectDescriptor ProjectDescriptor;
			FText OutError;

			// FPaths::GetProjectFilePath() return the  path of project's .uproject file
			ProjectDescriptor.Load(ProjectFilePath, OutError);
			m_GameModuleName = ProjectDescriptor.Modules[0].Name.ToString(); // get the first module name in file ProjectName.uproject 
			return true;
		}
	}
	return false;
}

void FLuaGenerator::Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase)
{
	UE_LOG(LogLuaGenerator, Warning, TEXT("Initialize"));
}


void FLuaGenerator::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	//UE_LOG(LogLuaGenerator, Warning, TEXT("ExportClass"));
}


void FLuaGenerator::FinishExport()
{
	UE_LOG(LogLuaGenerator, Warning, TEXT("FinishExport"));
}


FString FLuaGenerator::GetGeneratorName() const
{
	UE_LOG(LogLuaGenerator, Warning, TEXT("GetGeneratorName"));
	return FString("LuaGenerator");
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLuaGenerator, LuaGenerator)