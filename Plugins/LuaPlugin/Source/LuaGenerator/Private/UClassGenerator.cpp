#include "UClassGenerator.h"
#include "UnrealType.h"
#include "Templates/Casts.h"
#include "Misc/FileHelper.h"
#include "UObjectIterator.h"
#include "ScriptGeneratorManager.h"

FUClassGenerator::FUClassGenerator(UClass *InClass, const FString &InOutDir, const FString& HeaderFile) 
	: IScriptGenerator(NS_LuaGenerator::EUClass, InOutDir)
	, m_pClass(InClass)
	, m_HeaderFileName(HeaderFile)
{
	m_LuaFuncReg.Init(GetClassName());
}

FUClassGenerator::~FUClassGenerator()
{

}

bool FUClassGenerator::CanExport() const
{
	return !g_LuaConfigManager->NotSuportClasses.Contains(GetClassName());
}

void FUClassGenerator::ExportToMemory()
{
	ExportDataMembersToMemory();
	ExportFunctionMembersToMemory();
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
	return FString("U")+m_pClass->GetName();
}

FString FUClassGenerator::GetFileName() const
{
	return GetClassName() +g_LuaConfigManager->ClassScriptHeaderSuffix;
}

FString FUClassGenerator::GetRegName() const
{
	return FString::Printf(TEXT("%s_Lib"), *GetClassName());
}

void FUClassGenerator::ExportDataMembersToMemory()
{

}

void FUClassGenerator::ExportFunctionMembersToMemory()
{
	for (TFieldIterator<UFunction> FuncIt(m_pClass /*, EFieldIteratorFlags::ExcludeSuper*/); FuncIt; ++FuncIt)
	{
		UFunction* Function = *FuncIt;
		if (NS_LuaGenerator::CanExportFunction(Function) && CanExportFunction(Function) && FExportFuncMemberInfo::CanExportFunction(Function))
		{
			m_LuaFuncReg.AddFunctionMember(FExportFuncMemberInfo::CreateFunctionMemberInfo(Function));
		}
	}
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
	return m_LuaFuncReg.GetFuncContents();
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

void FUClassGenerator::GetParentNames(TArray<FString> &OutParentNames) const
{

}

