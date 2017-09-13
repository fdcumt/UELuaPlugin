#pragma once
#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "ILuaGenerator.h"
#include "GeneratorDefine.h"
#include "LuaScriptGenerator.h"

class FLuaGenerator : public ILuaGenerator
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	/** ILuaglueGenerator interface */
	virtual FString GetGeneratedCodeModuleName() const override;
	virtual bool ShouldExportClassesForModule(const FString& ModuleName, EBuildModuleType::Type ModuleType, const FString& ModuleGeneratedIncludeDirectory) const override;
	virtual bool SupportsTarget(const FString& TargetName) const override;
	virtual void Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase) override;
	virtual void ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged) override;
	virtual void FinishExport() override;
	virtual FString GetGeneratorName() const override;

protected:
	TArray<FString> m_SupportedModules;
	FLuaScriptGenerator *m_LuaScriptGenerator;
};