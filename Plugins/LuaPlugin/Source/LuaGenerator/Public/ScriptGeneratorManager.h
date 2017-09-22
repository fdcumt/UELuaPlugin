#pragma once
#include "GeneratorDefine.h"
#include "IScriptGenerator.h"

class FScriptGeneratorManager
{
public:
	FScriptGeneratorManager();
	~FScriptGeneratorManager();

public:
	void Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase);
	void ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged);
	void FinishExport();

private:
	bool CanExportClass(IScriptGenerator *InGenerator) const ;

private: // config class
	void ExportConfigClasses();
	void ExportConfigClass(const FConfigClass& ClassItem);
	void ParseConfigClass(const FString &&FileName, TArray<FConfigClass> &OutConfigClasses);
	void AddGeneratorToMap(IScriptGenerator *InGenerator);


private:
	FString m_OutDir;
	FString m_RootLocalPath;
	FString m_RootBuildPath;
	FString m_IncludeBase;
	TMap<FString, IScriptGenerator*> m_Generators;
};
