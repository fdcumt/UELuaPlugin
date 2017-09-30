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
	m_FileName.Empty();
	m_FileContent.Empty();
	m_FunctionNames.Empty();
	m_FileName = InClass->GetName() + NS_LuaGenerator::ClassScriptHeaderSuffix; 
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
	GenerateScriptHeader(m_FileContent);
	GenerateFunctions(m_FileContent);
	GenerateRegister(m_FileContent);
	GenerateScriptTail(m_FileContent);
}

void FUClassGenerator::SaveToFile()
{
	FString fileName = m_OutDir/m_pClass->GetName() + NS_LuaGenerator::ClassScriptHeaderSuffix;
	if (!FFileHelper::SaveStringToFile(m_FileContent, *fileName))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("Failed to save header export:%s"), *fileName);
	}
}

FString FUClassGenerator::GetClassName() const
{
	return m_pClass->GetName();
}

void FUClassGenerator::GenerateScriptHeader(FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT("#pragma once"));
	OutStr += EndLinePrintf(TEXT("PRAGMA_DISABLE_DEPRECATION_WARNINGS"));
	OutStr += EndLinePrintf(TEXT(""));
}

void FUClassGenerator::GenerateIncludeHeader(FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT("#include \"%s\""), *m_HeaderFileName);
}

void FUClassGenerator::GenerateFunctions(FString &OutStr)
{
	for (TFieldIterator<UFunction> FuncIt(m_pClass); FuncIt; ++FuncIt)
	{
		if (CanExportFunction(*FuncIt))
		{
			AddFunctionToRegister(*FuncIt);
			GenerateSingleFunction(*FuncIt, OutStr);
		}
	}
}

void FUClassGenerator::GenerateSingleFunction(UFunction *InFunction, FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT(""));
	OutStr += EndLinePrintf(TEXT("static int32 %s_%s(lua_State* L)"), *m_pClass->GetName(), *InFunction->GetFName().ToString());
	OutStr += EndLinePrintf(TEXT("{"));
	GenerateFunctionParams(InFunction, OutStr);
	OutStr += EndLinePrintf(TEXT("}"));
}

void FUClassGenerator::AddFunctionToRegister(UFunction *InFunction)
{
	m_FunctionNames.Add(InFunction->GetName());
}

void FUClassGenerator::GenerateFunctionParam(UProperty *InParam, int32 InIndex, FString &OutStr)
{
	FString propertyType = GetPropertyType(InParam, CPPF_ArgumentOrReturnValue);
	//DebugLog(TEXT("GetPropertyType type:%s"), *propertyType);

	if (InParam->IsA(UIntProperty::StaticClass()) ||
		InParam->IsA(UUInt32Property::StaticClass()) ||
		InParam->IsA(UInt64Property::StaticClass()) ||
		InParam->IsA(UUInt16Property::StaticClass()))
	{
		//DebugLog(TEXT("Function param type:%s"), TEXT("int32"));
	}
	else if (InParam->IsA(UFloatProperty::StaticClass()))
	{
		//DebugLog(TEXT("Function param type:%s"), TEXT("float"));
	}
	else if (InParam->IsA(UStrProperty::StaticClass()))
	{
		//DebugLog(TEXT("Function param type:%s"), TEXT("FString"));
	}
	else if (InParam->IsA(UNameProperty::StaticClass()))
	{
		//DebugLog(TEXT("Function param type:%s"), TEXT("FName"));
	}
	else if (InParam->IsA(UBoolProperty::StaticClass()))
	{
		//DebugLog(TEXT("Function param type:%s"), TEXT("bool"));
	}
	else if (InParam->IsA(UStructProperty::StaticClass()))
	{
		UStructProperty* StructProp = CastChecked<UStructProperty>(InParam);
		//DebugLog(TEXT("Function param type:%s"), *StructProp->Struct->GetName());
	}
	else if (InParam->IsA(UClassProperty::StaticClass()))
	{

	}
	else if (InParam->IsA(UObjectPropertyBase::StaticClass()))
	{
		
	}
}

void FUClassGenerator::GenerateFunctionParams(UFunction *InFunction, FString &OutStr)
{
	int32 index = 0;
	for (TFieldIterator<UProperty> ParamIt(InFunction); ParamIt; ++ParamIt, ++index)
	{
		GenerateFunctionParam(*ParamIt, index, OutStr);
	}
}

void FUClassGenerator::GeneratorCheckParamsNumCode(UFunction *InFunction, FString &OutStr)
{

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

void FUClassGenerator::GenerateRegister(FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT(""));
	OutStr += EndLinePrintf(TEXT("static const luaL_Reg %s_Lib[] ="), *m_pClass->GetName());
	OutStr += EndLinePrintf(TEXT("{"));

	for (const FString &FunctionName : m_FunctionNames)
	{
		GenerateRegisterItem(FunctionName, OutStr);
	}

	OutStr += EndLinePrintf(TEXT("\t{ NULL, NULL }"));
	OutStr += EndLinePrintf(TEXT("};"));
}

void FUClassGenerator::GenerateRegisterItem(const FString &InFunctionName, FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT("\t{ \"%s\", %s},"), *InFunctionName, *GenerateRegisterFuncName(*InFunctionName, *m_pClass->GetName()) );
}

FString FUClassGenerator::GenerateRegisterFuncName(const FString &InFunctionName, const FString &ClassName)
{
	return ClassName + TEXT("_") + InFunctionName;
}

bool FUClassGenerator::CanExportFunction(UFunction *InFunction)
{
	if ( (InFunction->FunctionFlags&FUNC_Public) && (InFunction->FunctionFlags & FUNC_RequiredAPI))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void FUClassGenerator::GenerateScriptTail(FString &OutStr)
{

	OutStr += EndLinePrintf(TEXT(""));
	OutStr += EndLinePrintf(TEXT("PRAGMA_ENABLE_DEPRECATION_WARNINGS"));
}

void FUClassGenerator::GetParentNames(TArray<FString> &OutParentNames) const
{

}

FUClassGeneratorConfig FUClassGenerator::m_ClassConfig;

