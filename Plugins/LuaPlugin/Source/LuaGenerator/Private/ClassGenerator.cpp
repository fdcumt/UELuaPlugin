#include "ClassGenerator.h"
#include "UnrealType.h"
#include "GeneratorDefine.h"
#include "Templates/Casts.h"
#include "Misc/FileHelper.h"
#include "UObjectIterator.h"

void FClassGeneratorConfig::Init()
{
	FString ConfigFilePath = NS_LuaGenerator::ProjectPath / NS_LuaGenerator::LuaConfigFileRelativePath;
	GConfig->GetArray(NS_LuaGenerator::NotSupportClassSection, NS_LuaGenerator::NotSupportClassKey, m_NotSuportClasses, ConfigFilePath);
}

bool FClassGeneratorConfig::CanExport(const FString &InClassName)
{
	return !m_NotSuportClasses.Contains(InClassName);
}

IScriptGenerator* FClassGenerator::CreateGenerator(UObject *InObj, const FString &OutDir)
{
	UClass *pClass = Cast<UClass>(InObj);

	if (pClass)
	{
		return new FClassGenerator(pClass, OutDir);
	}
	else
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("FClassGenerator::CreateGenerator error"));
		return nullptr;
	}
}

FClassGenerator::FClassGenerator(UClass *InClass, const FString &InOutDir)
{
	m_FileName.Empty();
	m_FileContent.Empty();
	m_FunctionNames.Empty();

	m_pClass = InClass;
	m_OutDir = InOutDir;
	m_FileName = InClass->GetFName().ToString() + NS_LuaGenerator::ClassScriptHeaderSuffix; 
}

FClassGenerator::~FClassGenerator()
{

}

bool FClassGenerator::CanExport()
{
	return m_ClassConfig.CanExport(m_pClass->GetFName().ToString());
}

void FClassGenerator::Export()
{
	GenerateScriptHeader(m_FileContent);
	GenerateFunctions(m_FileContent);
	GenerateRegister(m_FileContent);
	GenerateScriptTail(m_FileContent);
	SaveToFile();
}

void FClassGenerator::SaveToFile()
{
	FString fileName = m_OutDir/m_pClass->GetFName().ToString() + NS_LuaGenerator::ClassScriptHeaderSuffix;
	if (!FFileHelper::SaveStringToFile(m_FileContent, *fileName))
	{
		UE_LOG(LogLuaGenerator, Warning, TEXT("Failed to save header export:%s"), *fileName);
	}
}

void FClassGenerator::GenerateScriptHeader(FString &OutStr)
{
	OutStr += FString::Printf(TEXT("#pragma once\r\n"));
	OutStr += FString::Printf(TEXT("PRAGMA_DISABLE_DEPRECATION_WARNINGS\r\n\r\n"));
}

void FClassGenerator::GenerateFunctions(FString &OutStr)
{
	for (TFieldIterator<UFunction> FuncIt(m_pClass); FuncIt; ++FuncIt)
	{
	
		AddFunctionToRegister(*FuncIt);
		GenerateSingleFunction(*FuncIt, OutStr);

// 		for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
// 		{
// 			UProperty *pParam = *ParamIt;
// 			UE_LOG(LogLuaGenerator, Warning, TEXT("ExportClass function param name: %s"), *pParam->GetFName().ToString());
// 		}
	}
}

void FClassGenerator::GenerateSingleFunction(UFunction *InFunction, FString &OutStr)
{
	OutStr += StringPrintfWithEndLine(TEXT(""));
	OutStr += StringPrintfWithEndLine(TEXT("static int32 %s_%s(lua_State* L)"), *m_pClass->GetFName().ToString(), *InFunction->GetFName().ToString());
	OutStr += StringPrintfWithEndLine(TEXT("{"));
	OutStr += StringPrintfWithEndLine(TEXT("}"));
}

void FClassGenerator::AddFunctionToRegister(UFunction *InFunction)
{
	m_FunctionNames.Add(InFunction->GetFName().ToString());
}

void FClassGenerator::GenerateRegister(FString &OutStr)
{
	OutStr += FString::Printf(TEXT("\r\n"));
	OutStr += FString::Printf(TEXT("static const luaL_Reg %s_Lib[] =\r\n"), *m_pClass->GetFName().ToString());
	OutStr += FString::Printf(TEXT("{\r\n"));

	for (const FString &FunctionName : m_FunctionNames)
	{
		GenerateRegisterItem(FunctionName, OutStr);
	}

	OutStr += FString::Printf(TEXT("\t{ NULL, NULL }\r\n"));
	OutStr += FString::Printf(TEXT("};\r\n"));
}

void FClassGenerator::GenerateRegisterItem(const FString &InFunctionName, FString &OutStr)
{
	OutStr += FString::Printf(TEXT("\t{ \"%s\", %s},\r\n"), *InFunctionName, *GenerateRegisterFuncName(*InFunctionName, *m_pClass->GetFName().ToString()) );
}

FString FClassGenerator::GenerateRegisterFuncName(const FString &InFunctionName, const FString &ClassName)
{
	return ClassName + TEXT("_") + InFunctionName;
}

void FClassGenerator::GenerateScriptTail(FString &OutStr)
{
	OutStr += FString::Printf(TEXT("\r\n"));
	OutStr += FString::Printf(TEXT("PRAGMA_ENABLE_DEPRECATION_WARNINGS\r\n"));
	OutStr += FString::Printf(TEXT("\r\n"));
}

FClassGeneratorConfig FClassGenerator::m_ClassConfig;

