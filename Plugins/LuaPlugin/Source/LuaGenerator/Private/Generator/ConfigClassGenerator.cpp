#include "ConfigClassGenerator.h"
#include "Misc/FileHelper.h"

IScriptGenerator* FConfigClassGenerator::CreateGenerator(const FConfigClass& ClassItem, const FString &OutDir)
{
	return new FConfigClassGenerator(ClassItem, OutDir);
}

FConfigClassGenerator::FConfigClassGenerator(const FConfigClass &ConfigClass, const FString &OutDir)
	: IScriptGenerator(NS_LuaGenerator::EConfigClass, OutDir)
	, m_ConfigClass(ConfigClass)
{

}

FConfigClassGenerator::~FConfigClassGenerator()
{

}

FString FConfigClassGenerator::GetKey() const
{
	return m_ConfigClass.GetClassName();
}

FString FConfigClassGenerator::GetClassName() const
{
	return m_ConfigClass.GetClassName();
}

FString FConfigClassGenerator::GetFileName() const
{
	return m_ConfigClass.GetClassName() + g_LuaConfigManager->ClassScriptHeaderSuffix;
}

FString FConfigClassGenerator::GetRegName() const
{
	return m_ConfigClass.GetRegLibName();
}

bool FConfigClassGenerator::CanExport() const
{
	return true;
}

void FConfigClassGenerator::ExportToMemory()
{

}

void FConfigClassGenerator::SaveToFile()
{
	FString fileName = m_OutDir/ GetFileName();
	FString fileContent;
	DebugProcedure(TEXT("FConfigClassGenerator::SaveToFile:%s"), *fileName);
	Unity(fileContent);
	DebugProcedure(TEXT("FConfigClassGenerator::SaveToFile: unity finish"));
	if (!FFileHelper::SaveStringToFile(fileContent, *fileName))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("Failed to save header export:%s"), *fileName);
	}
}

void FConfigClassGenerator::GetParentNames(TArray<FString> &OutParentNames) const
{
	OutParentNames.Append(m_ConfigClass.ParentNames);
}

void FConfigClassGenerator::Unity(FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT("#pragma once"));
	OutStr += EndLinePrintf(TEXT("#include \"LuaUtil.h\""));
	OutStr += m_ConfigClass.GetIncludeFilesChunk();
	OutStr += m_ConfigClass.GetFunctionsChunk();
	OutStr += m_ConfigClass.GetRegLibChunk();
}