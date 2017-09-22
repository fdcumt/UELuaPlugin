#include "ConfigClassGenerator.h"
#include "Misc/FileHelper.h"

IScriptGenerator* FConfigClassGenerator::CreateGenerator(const FConfigClass& ClassItem, const FString &OutDir)
{
	return new FConfigClassGenerator(ClassItem, OutDir);
}

FConfigClassGenerator::FConfigClassGenerator(const FConfigClass &ConfigClass, const FString &OutDir)
	: IScriptGenerator(OutDir)
	, m_ConfigClass(ConfigClass)
{

}

FConfigClassGenerator::~FConfigClassGenerator()
{

}

FString FConfigClassGenerator::GetKey() const
{
	return m_ConfigClass.Name;
}

bool FConfigClassGenerator::CanExport() const
{
	return true;
}

void FConfigClassGenerator::Export()
{
	GeneratorScriptName(m_FileName);
	GenerateScriptHeader(m_ScriptHeader);
	GenerateFunctions(m_Functions);
	GenerateRegVariableHeader(m_RegVariableHeader);
	GenerateRegVariableItems(m_RegVariableItems);
	GenerateRegVariableTail(m_RegVariableTail);
	GenerateScriptTail(m_ScriptTail);
	SaveToFile();
}

void FConfigClassGenerator::SaveToFile()
{
	FString fileName = m_OutDir/m_FileName;
	FString fileContent;
	Unity(fileContent);
	if (!FFileHelper::SaveStringToFile(fileContent, *fileName))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("Failed to save header export:%s"), *fileName);
	}
}

void FConfigClassGenerator::GenerateScriptHeader(FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT("#pragma once"));
	OutStr += EndLinePrintf(TEXT("PRAGMA_DISABLE_DEPRECATION_WARNINGS"));
	OutStr += EndLinePrintf(TEXT(""));
}

void FConfigClassGenerator::GenerateFunctions(TMap<FString, FString> &OutFunctionMap)
{
	for (const FConfigFunction &FuncItem : m_ConfigClass.Functions)
	{
		FString StrKey;
		FString StrValue;
		GenerateFunctionKey(FuncItem, StrKey);
		GenerateFunctionValue(FuncItem, StrValue);
		AddToMap("function map", StrKey, StrValue, OutFunctionMap);
	}
}

void FConfigClassGenerator::GenerateFunctionKey(const FConfigFunction &FuncItem, FString &OutKey)
{
	OutKey = FuncItem.Name;
}

void FConfigClassGenerator::GenerateFunctionValue(const FConfigFunction &FuncItem, FString &OutStr)
{
	int32 index = 0;
	FString FunctionName;
	FString ParamList;
	FString strComma(", ");
	FString ReturnValueName("RetValue");
	GeneratorFunctionName(FuncItem, FunctionName);
	OutStr += EndLinePrintf(TEXT(""));
	OutStr += EndLinePrintf(TEXT("static int32 %s(lua_State* L)"), *FunctionName);
	OutStr += EndLinePrintf(TEXT("{"));

	if (!FuncItem.bStatic)
	{
		OutStr += EndLinePrintf(TEXT("\t%s *pObj = FLuaUtil::TouserCppClassType<%s*>(\"%s\");"), *m_ConfigClass.Name, *m_ConfigClass.Name, *m_ConfigClass.Name);
	}

	for (const FString &ParamItemType : FuncItem.ParamTypes)
	{
		FString ParamName = FString::Printf(TEXT("param_%d"), index);
		OutStr += EndLinePrintf(TEXT("\t%s %s;"), *ParamItemType, *ParamName);
		OutStr += EndLinePrintf(TEXT("\tFLuaUtil::Pop(FLuaClassType<%s>(%s, \"%s\"));"), *ParamItemType, *ParamName, *ParamItemType);

		if (index>0)
		{
			ParamList += strComma;
		}
		ParamList += ParamName;
	}


	if (FuncItem.RetType!=FString("void"))
	{
		OutStr += FString::Printf(TEXT("\t%s %s = "), *FuncItem.RetType, *ReturnValueName);
	}
	else
	{
		OutStr += FString::Printf(TEXT("\t"));
	}

	if (!FuncItem.bStatic)
	{
		OutStr += EndLinePrintf(TEXT("pObj->%s(%s);"), *FuncItem.Name, *ParamList);
	}
	else
	{
		OutStr += EndLinePrintf(TEXT("%s::%s(%s);"), *m_ConfigClass.Name, *FuncItem.Name, *ParamList);
	}

	if (FuncItem.RetType != FString("void"))
	{
		OutStr += EndLinePrintf(TEXT("\tPush(FLuaClassType<%s>(%s, \"%s\"));"), *FuncItem.RetType, *ReturnValueName, *FuncItem.RetType);
	}
	
	OutStr += EndLinePrintf(TEXT("\treturn 1;"));
	OutStr += EndLinePrintf(TEXT("}"));
}

void FConfigClassGenerator::GenerateRegVariableHeader(FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT(""));
	OutStr += EndLinePrintf(TEXT(""));
	OutStr += EndLinePrintf(TEXT("static const luaL_Reg %s_Lib[] ="), *m_ConfigClass.Name);
	OutStr += EndLinePrintf(TEXT("{"));
}

void FConfigClassGenerator::GenerateRegVariableItems(TMap<FString, FString> &OutMap)
{
	for (const FConfigFunction &FuncItem : m_ConfigClass.Functions)
	{
		FString StrKey;
		FString StrValue;
		GenerateRegVariableItemKey(FuncItem, StrKey);
		GenerateRegVariableItemValue(FuncItem, StrValue);
		AddToMap("register lib map", StrKey, StrValue, OutMap);
	}
}

void FConfigClassGenerator::GenerateRegVariableItemKey(const FConfigFunction &FuncItem, FString &OutStr)
{
	OutStr = FuncItem.Name;
}

void FConfigClassGenerator::GenerateRegVariableItemValue(const FConfigFunction &FuncItem, FString &OutStr)
{
	FString FuncName;
	GeneratorFunctionName(FuncItem, FuncName);
	OutStr += EndLinePrintf(TEXT("\t{\"%s\", %s},"), *FuncItem.Name, *FuncName);
}

void FConfigClassGenerator::GenerateRegVariableTail(FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT("\t{ NULL, NULL }"));
	OutStr += EndLinePrintf(TEXT("};"));
}

void FConfigClassGenerator::GenerateScriptTail(FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT(""));
	OutStr += EndLinePrintf(TEXT("PRAGMA_ENABLE_DEPRECATION_WARNINGS"));
	OutStr += EndLinePrintf(TEXT(""));
}

void FConfigClassGenerator::AddToMap(const char *MapType, const FString &InKey, const FString &InValue, TMap<FString, FString> &InMap)
{
	FString *result = InMap.Find(InKey);
	if (result==nullptr)
	{
		InMap.Add(InKey, InValue);
	}
	else
	{
		GeneratorLog(Error, TEXT("%s, %s has already have the key %s, can not add repeat!"), *m_ConfigClass.Name, ANSI_TO_TCHAR(MapType), *InKey);
	}
}

void FConfigClassGenerator::GeneratorFunctionName(const FConfigFunction &FuncItem, FString &OutStr)
{
	OutStr = FString::Printf(TEXT("%s_%s"), *m_ConfigClass.Name, *FuncItem.Name);
}

void FConfigClassGenerator::Unity(FString &OutStr)
{
	OutStr += m_ScriptHeader;
	
	for (auto &Item : m_Functions)
	{
		OutStr += Item.Value;
	}

	OutStr += m_RegVariableHeader;

	for (auto &Item : m_RegVariableItems)
	{
		OutStr += Item.Value;
	}
	OutStr += m_RegVariableTail;
	OutStr += m_ScriptTail;
}

void FConfigClassGenerator::GeneratorScriptName(FString &OutStr)
{
	OutStr += FString::Printf(TEXT("%s.Script.h"), *m_ConfigClass.Name);
}
