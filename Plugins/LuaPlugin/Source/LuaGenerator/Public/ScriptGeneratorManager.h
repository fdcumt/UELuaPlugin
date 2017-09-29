#pragma once
#include "GeneratorDefine.h"
#include "IScriptGenerator.h"
#include "ConfigClassDefine.h"
#include "ClassParentsManager.h"

class FScriptGeneratorManager
{
public:
	FScriptGeneratorManager();
	~FScriptGeneratorManager();

public:
	void Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase);
	void ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged);
	void FinishExport();

public:
	bool ContainClassName(const FString &ClassName);
	TArray<FString> GetParentNames(const FString &ClassName);
	IScriptGenerator* GetGenerator(const FString &ClassName);

private:
	bool CanExportClass(IScriptGenerator *InGenerator) const ;
	void ExportExtrasToMemory();
	void AdjustBeforeSaveToFile();
	void InitClassParentManager();

private: // save to file
	void SaveToFiles();
	void SaveConfigClassesToFiles();
	void InitConfig();

private: // finish export post
	void FinishExportPost();
	void GenerateAndSaveAllHeaderFile();
	void GererateLoadAllDefineFile();

private: // config class
	void ExportConfigClasses();
	void ExportConfigClass(const FConfigClass& ClassItem);
	void ParseConfigClass(const FString &&FileName, TArray<FConfigClass> &OutConfigClasses);

private:
	void AddGeneratorToMap(IScriptGenerator *InGenerator);
	
private:
	FString m_OutDir;
	FString m_RootLocalPath;
	FString m_RootBuildPath;
	FString m_IncludeBase;
	TMap<FString, IScriptGenerator*> m_Generators;
	FClassParentManager m_ClassParentManager;
};
