#include "UStructGenerator.h"
#include "UnrealType.h"
#include "Templates/Casts.h"
#include "Misc/FileHelper.h"
#include "UObjectIterator.h"

IScriptGenerator* FUStructGenerator::CreateGenerator(UScriptStruct *InScriptStruct, const FString &InOutDir)
{
	return new FUStructGenerator(InScriptStruct, InOutDir);
}

FUStructGenerator::FUStructGenerator(UScriptStruct *InScriptStruct, const FString &InOutDir)
	: IScriptGenerator(NS_LuaGenerator::EUStruct, InOutDir)
	, m_pScriptStruct(InScriptStruct)
{
	m_LuaFuncReg.Init(GetClassName());
}

FUStructGenerator::~FUStructGenerator()
{

}

FString FUStructGenerator::GetKey() const
{
	return GetClassName();
}

FString FUStructGenerator::GetFileName() const
{
	return GetClassName()+g_LuaConfigManager->ClassScriptHeaderSuffix;
}

FString FUStructGenerator::GetRegName() const
{
	return FString::Printf(TEXT("%s_Lib"), *GetClassName());
}

bool FUStructGenerator::CanExport() const
{
	if (m_pScriptStruct->HasMetaData("DEPRECATED"))
	{
		return false;
	}

	if (!g_LuaConfigManager->SupportStructs.Contains(GetClassName()))
	{
		return false;
	}

	if (m_pScriptStruct->StructFlags & STRUCT_NoExport)
	{
		return false;
	}

	return true;
}

void FUStructGenerator::ExportToMemory()
{
	ExportDataMemberToMemory();
	ExportFunctionsToMemory();
	ExportExtraFuncsToMemory();
}

void FUStructGenerator::SaveToFile()
{
	DebugProcedure(TEXT("FUStructGenerator::SaveToFile"));
	FString FileContent;
	FString FilePathName = m_OutDir / GetFileName();
	DebugProcedure(TEXT("FUStructGenerator::SaveToFile:%s"), *GetFileName());
	FileContent += GetFuncContents();
	DebugProcedure(TEXT("FUStructGenerator::GetFuncContents"));

	FileContent += GetRegContents();
	DebugProcedure(TEXT("FUStructGenerator::GetRegContents"));


	if (!FFileHelper::SaveStringToFile(FileContent, *FilePathName))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("FUStructGenerator Failed to save export header:%s"), *GetFileName());
	}
}

FString FUStructGenerator::GetClassName() const
{
	return FString::Printf(TEXT("%s%s"), m_pScriptStruct->GetPrefixCPP(), *m_pScriptStruct->GetName());
}

void FUStructGenerator::GetParentNames(TArray<FString> &OutParentNames) const
{
	
}

void FUStructGenerator::ExportDataMemberToMemory()
{
	for (TFieldIterator<UProperty> PropertyIt(m_pScriptStruct/*, EFieldIteratorFlags::ExcludeSuper*/); PropertyIt; ++PropertyIt)
	{
		UProperty* pProperty = *PropertyIt;
		FExportDataMemberInfo DataMemberInfo = FExportDataMemberInfo::CreateExportDataMemberInfo(pProperty);
		if (NS_LuaGenerator::CanExportProperty(pProperty) && CanExportProperty(pProperty) && DataMemberInfo.CanExportDataMember(pProperty))
		{
			m_LuaFuncReg.AddDataMember(DataMemberInfo);
		}
	}
}

void FUStructGenerator::ExportFunctionsToMemory()
{
	for (TFieldIterator<UFunction> FuncIt(m_pScriptStruct /*, EFieldIteratorFlags::ExcludeSuper*/); FuncIt; ++FuncIt)
	{
		UFunction* Function = *FuncIt;
		if (CanExportFunction(Function) && FExportFuncMemberInfo::CanExportFunction(Function))
		{
			m_LuaFuncReg.AddFunctionMember(FExportFuncMemberInfo::CreateFunctionMemberInfo(Function));
		}
	}
}

void FUStructGenerator::ExportExtraFuncsToMemory()
{
	m_LuaFuncReg.AddExtraFuncMember(GenerateNewExportFunction());
	m_LuaFuncReg.AddExtraFuncMember(GenerateDestoryExportFunction());
}

bool FUStructGenerator::CanExportFunction(UFunction *InFunction)
{
	return true;
}

bool FUStructGenerator::CanExportProperty(UProperty *InProperty)
{
	return true;
}

FString FUStructGenerator::GetFuncContents()
{
	return m_LuaFuncReg.GetFuncContents();
}

FString FUStructGenerator::GetRegContents()
{
	return m_LuaFuncReg.GetRegLibContents();
}

FExtraFuncMemberInfo FUStructGenerator::GenerateNewExportFunction()
{
	FExtraFuncMemberInfo ExtraFuncNew;
	ExtraFuncNew.funcName = "New";
	FString &funcBody = ExtraFuncNew.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pStruct = new %s;"), *GetClassName(), *GetClassName());
	funcBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s*>(pStruct, \"%s\"));"), *GetClassName(), *GetClassName());
	funcBody += EndLinePrintf(TEXT("\treturn 1;"));

	return ExtraFuncNew;
}

FExtraFuncMemberInfo FUStructGenerator::GenerateDestoryExportFunction()
{
	FExtraFuncMemberInfo ExtraFuncDestory;
	ExtraFuncDestory.funcName = "Destory";
	FString &funcBody = ExtraFuncDestory.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pStruct = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *GetClassName(), *GetClassName(), *GetClassName());
	funcBody += EndLinePrintf(TEXT("\tif(pStruct) delete pStruct;"));
	funcBody += EndLinePrintf(TEXT("\tpStruct = nullptr;"));
	funcBody += EndLinePrintf(TEXT("\treturn 0;"));
	return ExtraFuncDestory;
}
