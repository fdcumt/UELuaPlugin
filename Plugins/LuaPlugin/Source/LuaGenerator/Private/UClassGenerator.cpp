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
	if (m_HeaderFileName.IsEmpty())
	{
		return false;
	}

	return !g_LuaConfigManager->NotSuportClasses.Contains(GetClassName());
}

void FUClassGenerator::ExportToMemory()
{
	ExportDataMembersToMemory();
	ExportFunctionMembersToMemory();
	ExportExtraFuncsToMemory();
}

void FUClassGenerator::SaveToFile()
{
	FString FileContent;
	FString FilePathName = m_OutDir/GetFileName();
	FileContent += GetFileHeader();
	FileContent += GetFileInclude();
	FileContent += GetFileFunctionContents();
	FileContent += GetFileRegContents();
	FileContent += GetFileTailContents();

	if (!FFileHelper::SaveStringToFile(FileContent, *FilePathName))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("Failed to save header export:%s"), *GetFileName());
	}
}

FString FUClassGenerator::GetClassName() const
{
	return FString::Printf(TEXT("%s%s"), m_pClass->GetPrefixCPP(), *m_pClass->GetName());
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
	for (TFieldIterator<UProperty> PropertyIt(m_pClass/*, EFieldIteratorFlags::ExcludeSuper*/); PropertyIt; ++PropertyIt)
	{
		UProperty* pProperty = *PropertyIt;
		FExportDataMemberInfo DataMemberInfo = FExportDataMemberInfo::CreateExportDataMemberInfo(pProperty);
		FExportDataMemberInfo CorrectDataInfo = FExportDataMemberInfo::GetCorrectDataMemberInfo(GetClassName(), DataMemberInfo);
		if (NS_LuaGenerator::CanExportProperty(pProperty) && CorrectDataInfo.CanExportDataMember(pProperty))
		{
			m_LuaFuncReg.AddDataMember(CorrectDataInfo);
		}
	}
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

void FUClassGenerator::ExportExtraFuncsToMemory()
{
	m_LuaFuncReg.AddExtraFuncMember(GenerateNewExportFunction());
}

FExtraFuncMemberInfo FUClassGenerator::GenerateNewExportFunction()
{
	FExtraFuncMemberInfo ExtraFuncNew;
	ExtraFuncNew.funcName = "New";
	FString &funcBody = ExtraFuncNew.funcBody;
	funcBody += EndLinePrintf(TEXT("\tUObject* Outer = FLuaUtil::TouserData<UObject*>(InLuaState, 1, \"UObject\");"));
	funcBody += EndLinePrintf(TEXT("\tFName Name = FName(luaL_checkstring(InLuaState, 2));"));
	funcBody += EndLinePrintf(TEXT("\t%s* pObj = NewObject<%s>(Outer, Name);"), *GetClassName(), *GetClassName());
	funcBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s*>(pObj, \"%s\"));"), *GetClassName(), *GetClassName());
	funcBody += EndLinePrintf(TEXT("\treturn 1;"));

	return ExtraFuncNew;
}

FString FUClassGenerator::GetFileHeader()
{
	FString StrContent;
	StrContent += EndLinePrintf(TEXT("#pragma once"));
	StrContent += EndLinePrintf(TEXT("PRAGMA_DISABLE_DEPRECATION_WARNINGS"));
	return StrContent;
}

FString FUClassGenerator::GetFileInclude()
{
	FString StrContent;
	if (!m_HeaderFileName.IsEmpty())
	{
		StrContent += EndLinePrintf(TEXT("#include \"%s\""), *m_HeaderFileName);
	}
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

FString FUClassGenerator::GetFileTailContents()
{
	FString StrContent;
	StrContent += EndLinePrintf(TEXT(""));
	StrContent += EndLinePrintf(TEXT("PRAGMA_ENABLE_DEPRECATION_WARNINGS"));
	return StrContent;
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

