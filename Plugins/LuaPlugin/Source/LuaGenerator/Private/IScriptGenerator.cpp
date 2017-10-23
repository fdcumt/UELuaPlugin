#include "IScriptGenerator.h"

IScriptGenerator::IScriptGenerator(NS_LuaGenerator::E_GeneratorType InType, const FString &OutDir)
	: m_eClassType(InType)
	, m_OutDir(OutDir)
{

}

IScriptGenerator::~IScriptGenerator()
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

