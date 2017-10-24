#include "IScriptGenerator.h"
#include "GeneratorDefine.h"
#include "LuaConfigManager.h"

IScriptGenerator::IScriptGenerator(NS_LuaGenerator::E_GeneratorType InType, const FString &OutDir)
	: m_eClassType(InType)
	, m_OutDir(OutDir)
{

}

IScriptGenerator::~IScriptGenerator()
{

}

FString IScriptGenerator::GetFileName() const
{
	return GetClassName() + g_LuaConfigManager->ClassScriptHeaderSuffix;
}

FString IScriptGenerator::GetRegName() const
{
	return FString::Printf(TEXT("%s_Lib"), *GetClassName());
}

void IScriptGenerator::GetParentNames(TArray<FString> &OutParentNames) const
{

}

FString IScriptGenerator::GetFileHeader()
{
	FString StrContent;
	StrContent += EndLinePrintf(TEXT("#pragma once"));
	StrContent += EndLinePrintf(TEXT("PRAGMA_DISABLE_DEPRECATION_WARNINGS"));
	return StrContent;
}

FString IScriptGenerator::GetFileTail()
{
	FString StrContent;
	StrContent += EndLinePrintf(TEXT(""));
	StrContent += EndLinePrintf(TEXT("PRAGMA_ENABLE_DEPRECATION_WARNINGS"));
	return StrContent;
}

