#pragma once

class FLuaScriptGenerator
{
public:
	FLuaScriptGenerator();
	~FLuaScriptGenerator();

public:
	void Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase);
	void ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged);
	void FinishExport();

private:
	bool CanExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged) const ;
	void ExportClassImpl(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged);

private:
	TSet<FName> m_ExportedClasses;
};
