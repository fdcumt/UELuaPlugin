#include "UClassGenerator.h"
#include "UnrealType.h"
#include "Templates/Casts.h"
#include "Misc/FileHelper.h"
#include "UObjectIterator.h"
#include "ScriptGeneratorManager.h"

void FUClassGeneratorConfig::Init()
{
	FString ConfigFilePath = NS_LuaGenerator::ProjectPath / NS_LuaGenerator::LuaConfigFileRelativePath;
	GConfig->GetArray(NS_LuaGenerator::NotSupportClassSection, NS_LuaGenerator::NotSupportClassKey, m_NotSuportClasses, ConfigFilePath);
}

bool FUClassGeneratorConfig::CanExport(const FString &InClassName)
{
	return !m_NotSuportClasses.Contains(InClassName);
}

FUClassGenerator::FUClassGenerator(UClass *InClass, const FString &InOutDir, const FString& HeaderFile) 
	: IScriptGenerator(NS_LuaGenerator::EUClass, InOutDir)
	, m_pClass(InClass)
	, m_HeaderFileName(HeaderFile)
{
	m_LuaFuncReg.Init(InClass->GetName());
}

FUClassGenerator::~FUClassGenerator()
{

}

bool FUClassGenerator::CanExport() const
{
	return m_ClassConfig.CanExport(m_pClass->GetName());
}

void FUClassGenerator::ExportToMemory()
{
	for (TFieldIterator<UFunction> FuncIt(m_pClass /*, EFieldIteratorFlags::ExcludeSuper*/); FuncIt; ++FuncIt)
	{
		UFunction* Function = *FuncIt;
		if (CanExportFunction(Function))
		{
			m_LuaFuncReg.AddFunction(GetFunctionInfo(Function));
		}
	}
}

void FUClassGenerator::SaveToFile()
{
	FString FileContent;
	FString FilePathName = m_OutDir/GetFileName();
	FileContent += GetFileHeader();
	FileContent += GetFileInclude();
	FileContent += GetFileFunctionContents();
	FileContent += GetFileRegContents();

	if (!FFileHelper::SaveStringToFile(FileContent, *FilePathName))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("Failed to save header export:%s"), *GetFileName());
	}
}

FString FUClassGenerator::GetClassName() const
{
	return m_pClass->GetName();
}

FString FUClassGenerator::GetFileName() const
{
	return m_pClass->GetName()+NS_LuaGenerator::ClassScriptHeaderSuffix;
}

FString FUClassGenerator::GetRegName() const
{
	return FString::Printf(TEXT("%s_Lib"), *m_pClass->GetName());
}

FString FUClassGenerator::GetFileHeader()
{
	FString StrContent;
	StrContent += EndLinePrintf(TEXT("#pragma once"));
	return StrContent;
}

FString FUClassGenerator::GetFileInclude()
{
	FString StrContent;
	StrContent += EndLinePrintf(TEXT("#include \"%s\""), *m_HeaderFileName);
	return StrContent;
}

FString FUClassGenerator::GetFileFunctionContents()
{
	return m_LuaFuncReg.GetFunctionContents();
}

FString FUClassGenerator::GetFileRegContents()
{
	return m_LuaFuncReg.GetRegLibContents();
}

bool FUClassGenerator::CanExportFunction(UFunction *InFunction)
{
	if (InFunction->FunctionFlags & FUNC_Delegate)
	{
		return false;
	}
	
	return true;
}

FExportFunctionInfo FUClassGenerator::GetFunctionInfo(UFunction* InFunction)
{
	FExportFunctionInfo functionInfo;
	functionInfo.InitByUFunction(InFunction);
	return functionInfo;
}

void FUClassGenerator::GetParentNames(TArray<FString> &OutParentNames) const
{

}

FUClassGeneratorConfig FUClassGenerator::m_ClassConfig;

