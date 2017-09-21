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
	IScriptGenerator* CreateGeneratorByType(NS_LuaGenerator::E_GeneratorType InType, UObject *InObj);
	bool CanExportClass(IScriptGenerator *InGenerator) const ;

private: // config class
	void ExportConfigClass();
	void ParseConfigClass(const FString &FileName, TArray<FConfigClass> &OutConfigClasses);



private:
	FString m_RootLocalPath;
	FString m_RootBuildPath;
	FString m_OutputDirectory;
	FString m_IncludeBase;
	TMap<FString, IScriptGenerator*> m_Generators;
};
