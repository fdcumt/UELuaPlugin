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
	TSet<FString> m_VariableTypes;
	FString m_LogContent;
	TSet<FString> m_PropertyType;

public:
	void Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase);
	void ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged);
	void FinishExport();

public:
	bool ContainClassName(const FString &ClassName);
	FString GetOutputDirectory() const { return m_OutDir; }
	TArray<FString> GetParentNames(const FString &ClassName);
	IScriptGenerator* GetGenerator(const FString &ClassName);
	void AddGeneratorToMap(IScriptGenerator *InGenerator);
	void AddGeneratorProperty(const FString &PlainName, UProperty *pProperty);

private:
	bool CanExportClass(IScriptGenerator *InGenerator) const ;
	void ExportExtrasToMemory();
	void AdjustBeforeSaveToFile();
	void InitClassParentManager();

private: // save to file
	void SaveToFiles();

private: // finish export post
	void FinishExportPost();
	void GenerateAndSaveAllHeaderFile();
	void GererateLoadAllDefineFile();

private: // config class
	void ExportConfigClasses();
	void ExportConfigClass(const FConfigClass& ClassItem);
	void ParseConfigClass(const FString &&FileName, TArray<FConfigClass> &OutConfigClasses);

private: // UStruct
	void ExportUStructs();
	void ExportGeneratorPropertys();
	void ExportUStruct(UScriptStruct *pScriptStruct);
	
private:
	FString m_OutDir;
	FString m_RootLocalPath;
	FString m_RootBuildPath;
	FString m_IncludeBase;
	TMap<FString, IScriptGenerator*> m_Generators;
	FClassParentManager m_ClassParentManager;
	TMap<FString, UProperty*> m_GeneratorPropertys;
};
