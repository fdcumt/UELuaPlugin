#include "UClassGenerator.h"
#include "UnrealType.h"
#include "Templates/Casts.h"
#include "Misc/FileHelper.h"
#include "UObjectIterator.h"

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

FString FUClassGenerator::GetPropertyType(UProperty *Property, uint32 PortFlags/*=0*/)
{
	static FString EnumDecl(TEXT("enum "));
	static FString StructDecl(TEXT("struct "));
	static FString ClassDecl(TEXT("class "));
	static FString TEnumAsByteDecl(TEXT("TEnumAsByte<enum "));
	static FString TSubclassOfDecl(TEXT("TSubclassOf<class "));

	FString PropertyType = Property->GetCPPType(NULL, PortFlags);
	if (Property->IsA(UArrayProperty::StaticClass()))
	{
		auto PropertyArr = Cast<UArrayProperty>(Property);
		FString inerTypeCpp = GetPropertyType(PropertyArr->Inner, CPPF_ArgumentOrReturnValue);
		if (inerTypeCpp == "EObjectTypeQuery")
			inerTypeCpp = "TEnumAsByte<EObjectTypeQuery> ";
		PropertyType = FString::Printf(TEXT("TArray<%s>"), *inerTypeCpp);
	}
	// Strip any forward declaration keywords
	if (PropertyType.StartsWith(EnumDecl) || PropertyType.StartsWith(StructDecl) || PropertyType.StartsWith(ClassDecl))
	{
		int FirstSpaceIndex = PropertyType.Find(TEXT(" "));
		PropertyType = PropertyType.Mid(FirstSpaceIndex + 1);
	}
	else if (PropertyType.StartsWith(TEnumAsByteDecl))
	{
		int FirstSpaceIndex = PropertyType.Find(TEXT(" "));
		PropertyType = TEXT("TEnumAsByte<") + PropertyType.Mid(FirstSpaceIndex + 1);
	}
	else if (PropertyType.StartsWith(TSubclassOfDecl))
	{
		int FirstSpaceIndex = PropertyType.Find(TEXT(" "));
		PropertyType = TEXT("TSubclassOf<") + PropertyType.Mid(FirstSpaceIndex + 1);
	}
	return PropertyType;
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
	StrContent += EndLinePrintf(TEXT(""));
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
	if (InFunction->FunctionFlags&FUNC_Delegate)
	{
		return false;
	}
	else
	{
		return true;
	}
}

FExportFunctionInfo FUClassGenerator::GetFunctionInfo(UFunction* InFunction)
{
	FExportFunctionInfo functionInfo;

	// init function name 
	functionInfo.FunctionName = InFunction->GetName();

	// init function return type
	UProperty *RetProperty = InFunction->GetReturnProperty();
	if (RetProperty)
	{
		functionInfo.ReturnType.Init(GetPropertyType(RetProperty));
	}
	else
	{
		functionInfo.ReturnType.Init(FString("void"));
	}
	
	// init function param list
	for (TFieldIterator<UProperty> ParamIt(InFunction); ParamIt; ++ParamIt)
	{
		UProperty* Param = *ParamIt;
		if (!(Param->GetPropertyFlags() & CPF_ReturnParm))
		{
			FVariableType FuncParam;
			FuncParam.Init(GetPropertyType(Param));
			functionInfo.FunctionParams.ParamTypes.Add(FuncParam);
		}
	}

	return functionInfo;
}

void FUClassGenerator::GetParentNames(TArray<FString> &OutParentNames) const
{

}

FUClassGeneratorConfig FUClassGenerator::m_ClassConfig;

