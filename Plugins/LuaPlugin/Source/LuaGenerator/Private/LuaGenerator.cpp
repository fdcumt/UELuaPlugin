#include "LuaGenerator.h"
#include "ProjectDescriptor.h"
#include "Features/IModularFeatures.h"

DEFINE_LOG_CATEGORY(LogLuaGenerator);

#define LOCTEXT_NAMESPACE "FLuaPluginModule"

void FLuaGenerator::StartupModule()
{
	UE_LOG(LogLuaGenerator, Warning, TEXT("StartupModule") );
	IModularFeatures::Get().RegisterModularFeature(TEXT("ScriptGenerator"), this);
}

void FLuaGenerator::ShutdownModule()
{
	UE_LOG(LogLuaGenerator, Warning, TEXT("ShutdownModule"));
	IModularFeatures::Get().UnregisterModularFeature(TEXT("ScriptGenerator"), this);

}

FString FLuaGenerator::GetGeneratedCodeModuleName() const
{
	UE_LOG(LogLuaGenerator, Warning, TEXT("GetGeneratedCodeModuleName"));
	return FString("MyProject");
}


bool FLuaGenerator::ShouldExportClassesForModule(const FString& ModuleName, EBuildModuleType::Type ModuleType, const FString& ModuleGeneratedIncludeDirectory) const
{
	UE_LOG(LogLuaGenerator, Warning, TEXT("ShouldExportClassesForModule, Module name:%s"), *ModuleName);
	return true;
}


bool FLuaGenerator::SupportsTarget(const FString& TargetName) const
{
	FProjectDescriptor ProjectDescriptor;
	FText OutError;
	ProjectDescriptor.Load(FPaths::GetProjectFilePath(), OutError);
	FString GameModuleName = ProjectDescriptor.Modules[0].Name.ToString();
	UE_LOG(LogLuaGenerator, Warning, TEXT("SupportsTarget, target name:%s, GameModuleName:%s, ProjectFilePath:%s"), *TargetName, *GameModuleName, *FPaths::GetProjectFilePath());
	return true;
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