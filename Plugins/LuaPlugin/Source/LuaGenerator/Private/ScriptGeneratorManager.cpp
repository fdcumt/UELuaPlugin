#include "ScriptGeneratorManager.h"
#include "UnrealType.h"
#include "ClassGenerator.h"
#include "GeneratorDefine.h"
#include "UObjectIterator.h"

FScriptGeneratorManager::FScriptGeneratorManager()
{

}

FScriptGeneratorManager::~FScriptGeneratorManager()
{

}

void FScriptGeneratorManager::Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase)
{
	m_RootLocalPath = RootLocalPath;
	m_RootBuildPath = RootBuildPath;
	m_OutputDirectory = OutputDirectory;
	m_IncludeBase = IncludeBase;

	UE_LOG(LogLuaGenerator, Warning, TEXT("m_RootLocalPath: %s"), *m_RootLocalPath);
	UE_LOG(LogLuaGenerator, Warning, TEXT("m_RootBuildPath: %s"), *m_RootBuildPath);
	UE_LOG(LogLuaGenerator, Warning, TEXT("m_OutputDirectory: %s"), *m_OutputDirectory);
}

void FScriptGeneratorManager::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	IScriptGenerator *pGenerator = CreateGeneratorByType(NS_LuaGenerator::EClass, Class);

	if (!CanExportClass(pGenerator))
	{
		return;
	}
	
	pGenerator->Export();
}

void FScriptGeneratorManager::FinishExport()
{
	for (TObjectIterator<UScriptStruct> It; It; ++It)
	{
		//UE_LOG(LogLuaGenerator, Warning, TEXT("UScriptStruct name: %s"),*It->GetFName().ToString());
	}
}

IScriptGenerator* FScriptGeneratorManager::CreateGeneratorByType(NS_LuaGenerator::E_GeneratorType InType, UObject *InObj)
{
	switch (InType)
	{
	case NS_LuaGenerator::EClass:
	{
		return FClassGenerator::CreateGenerator(InObj, m_OutputDirectory);
	}
	default:
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("FScriptGeneratorManager::CreateGeneratorByType error, not found generator type:%d"), int32(InType));
	}
	}

	return nullptr;
}

bool FScriptGeneratorManager::CanExportClass(IScriptGenerator *InGenerator) const
{
	return InGenerator && !m_Generators.Find(InGenerator->GetKey());
}
