#include "LuaScriptGenerator.h"
#include "GeneratorDefine.h"
#include "UObjectIterator.h"

FLuaScriptGenerator::FLuaScriptGenerator()
{

}

FLuaScriptGenerator::~FLuaScriptGenerator()
{

}

void FLuaScriptGenerator::Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase)
{

}

void FLuaScriptGenerator::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	if (CanExportClass(Class, SourceHeaderFilename, GeneratedHeaderFilename, bHasChanged))
	{
		ExportClassImpl(Class, SourceHeaderFilename, GeneratedHeaderFilename, bHasChanged);
	}
}

void FLuaScriptGenerator::FinishExport()
{
	for (TObjectIterator<UScriptStruct> It; It; ++It)
	{
		UE_LOG(LogLuaGenerator, Warning, TEXT("UScriptStruct name: %s"),*It->GetFName().ToString());
	}
}

bool FLuaScriptGenerator::CanExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged) const
{
	return 
	   (Class->ClassFlags & (CLASS_RequiredAPI | CLASS_MinimalAPI)) // export classes that exported in DLL symbols
	&& m_ExportedClasses.Contains(Class->GetFName()) // Don't export classes that have already been exported
	; 
}

void FLuaScriptGenerator::ExportClassImpl(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	m_ExportedClasses.Add(Class->GetFName());
}

