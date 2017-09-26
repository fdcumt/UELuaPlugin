#include "ConfigClassGenerator.h"
#include "Misc/FileHelper.h"

IScriptGenerator* FConfigClassGenerator::CreateGenerator(const FConfigClass& ClassItem, const FString &OutDir)
{
	return new FConfigClassGenerator(ClassItem, OutDir);
}

FConfigClassGenerator::FConfigClassGenerator(const FConfigClass &ConfigClass, const FString &OutDir)
	: IScriptGenerator(NS_LuaGenerator::EConfigClass, OutDir)
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

FString FConfigClassGenerator::GetFileName() const
{
	return m_FileName;
}

FString FConfigClassGenerator::GetRegName() const
{
	return GenerateRegVariableName();
}

bool FConfigClassGenerator::CanExport() const
{
	return true;
}

void FConfigClassGenerator::ExportToMemory()
{
	GeneratorScriptName(m_FileName);
	GenerateScriptHeader(m_ScriptHeader);
	GeneratorIncludeHeaders(m_IncludeFiles);
	GenerateDisableWarning(m_DisableWarning);
	GenerateFunctions(m_Functions);
	GenerateRegVariableHeader(m_RegVariableHeader);
	GenerateRegVariableItems(m_RegVariableItems);
	GenerateRegVariableTail(m_RegVariableTail);
	GenerateEnableWarning(m_EnableWarning);
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
}

void FConfigClassGenerator::GenerateDisableWarning(FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT(""));
	OutStr += EndLinePrintf(TEXT("PRAGMA_DISABLE_DEPRECATION_WARNINGS"));
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
	bool bAddComma = false;
	FString FunctionName;
	FString strParamNames;
	FString strComma(", ");
	FString ReturnValueName("RetValue");
	GeneratorFunctionName(FuncItem, FunctionName);
	OutStr += EndLinePrintf(TEXT(""));
	OutStr += EndLinePrintf(TEXT("static int32 %s(lua_State* InLuaState)"), *FunctionName);
	OutStr += EndLinePrintf(TEXT("{"));
	
	for (int32 i = FuncItem.ParamTypes.Num()-1; i>=0; --i)
	{
		const FString &ParamItemType = FuncItem.ParamTypes[i];
		FString ParamName = FString::Printf(TEXT("param_%d"), i);
		OutStr += EndLinePrintf(TEXT("\t%s %s;"), *ParamItemType, *ParamName);
		OutStr += EndLinePrintf(TEXT("\tFLuaUtil::Pop(InLuaState, FLuaClassType<%s>(%s, \"%s\"));"), *ParamItemType, *ParamName, *GeneratePureClassName(ParamItemType));

		if (!bAddComma)
		{
			bAddComma = true;
			strParamNames = ParamName;
		}
		else
		{
			strParamNames = ParamName + strComma + strParamNames;
		}
	}

	if (!FuncItem.bStatic)
	{
		OutStr += EndLinePrintf(TEXT("\t%s *pObj = FLuaUtil::TouserCppClassType<%s*>(InLuaState, \"%s\");"), *m_ConfigClass.Name, *m_ConfigClass.Name, *GeneratePureClassName(m_ConfigClass.Name));
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
		OutStr += EndLinePrintf(TEXT("pObj->%s(%s);"), *FuncItem.Name, *strParamNames);
	}
	else
	{
		OutStr += EndLinePrintf(TEXT("%s::%s(%s);"), *m_ConfigClass.Name, *FuncItem.Name, *strParamNames);
	}

	if (FuncItem.RetType != FString("void"))
	{
		OutStr += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(%s, \"%s\"));"), *FuncItem.RetType, *ReturnValueName, *GeneratePureClassName(FuncItem.RetType));
	}
	
	OutStr += EndLinePrintf(TEXT("\treturn 1;"));
	OutStr += EndLinePrintf(TEXT("}"));
}

void FConfigClassGenerator::GenerateRegVariableHeader(FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT(""));
	OutStr += EndLinePrintf(TEXT(""));
	OutStr += EndLinePrintf(TEXT("static const luaL_Reg %s[] ="), *GenerateRegVariableName());
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

void FConfigClassGenerator::GenerateEnableWarning(FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT(""));
	OutStr += EndLinePrintf(TEXT("PRAGMA_ENABLE_DEPRECATION_WARNINGS"));
}

void FConfigClassGenerator::GenerateScriptTail(FString &OutStr)
{
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

FString FConfigClassGenerator::GenerateRegVariableName() const 
{
	return FString::Printf(TEXT("%s_Lib"), *m_ConfigClass.Name);

}

FString FConfigClassGenerator::GeneratePureClassName(const FString& InClassName) const
{
	// remove the const,*,& from class name
	int32 BeginIndex = 0;
	int32 EndIndex = InClassName.Len()-1;

	while (BeginIndex<InClassName.Len())
	{ // remove the prefix
		if (InClassName[BeginIndex]==' ' || InClassName[BeginIndex]=='\t')
		{
			++BeginIndex;
		}
		else if (InClassName.StartsWith(ANSI_TO_TCHAR("const "), ESearchCase::CaseSensitive))
		{
			BeginIndex += 6;
		}
		else if (InClassName.StartsWith(ANSI_TO_TCHAR("class "), ESearchCase::CaseSensitive))
		{
			BeginIndex += 6;
		}
		else
		{
			break;
		}
	}

	while (EndIndex >= BeginIndex)
	{ // remove the suffix
		if (InClassName[EndIndex] == ' '  || InClassName[EndIndex] == '&')
		{
			--EndIndex;
		}
		else if (InClassName[EndIndex] == '\t' || InClassName[EndIndex] == '*')
		{
			--EndIndex;
		}
		else if (InClassName.EndsWith(ANSI_TO_TCHAR("const"), ESearchCase::CaseSensitive))
		{
			BeginIndex -= 5;
		}
		else
		{
			break;
		}
	}

	if (EndIndex >= BeginIndex)
	{
		return InClassName.Mid(BeginIndex, EndIndex - BeginIndex + 1);
	}
	else
	{
		return FString("");
	}
}

void FConfigClassGenerator::Unity(FString &OutStr)
{
	// script header
	OutStr += m_ScriptHeader;

	// include file names
	for (const FString &IncludeFile : m_IncludeFiles)
	{
		OutStr += IncludeFile;
	}

	// disable warning
	OutStr += m_DisableWarning;
	
	// functions
	for (auto &Item : m_Functions)
	{
		OutStr += Item.Value;
	}

	// register header
	OutStr += m_RegVariableHeader;

	// register vars
	for (auto &Item : m_RegVariableItems)
	{
		OutStr += Item.Value;
	}

	// register var tail
	OutStr += m_RegVariableTail;

	// enable warning
	OutStr += m_EnableWarning;

	// file tail
	OutStr += m_ScriptTail;
}

void FConfigClassGenerator::GeneratorScriptName(FString &OutStr)
{
	OutStr += FString::Printf(TEXT("%s.Script.h"), *m_ConfigClass.Name);
}

void FConfigClassGenerator::GeneratorIncludeHeaders(TArray<FString> &OutStr)
{
	OutStr.Add(EndLinePrintf(TEXT("#include \"LuaUtil.h\"")));

	for (const FString &IncludeFile : m_ConfigClass.IncludeHeaders)
	{
		FString strIncludeFile;
		GeneratorIncludeHeader(IncludeFile, strIncludeFile);
		OutStr.Add(strIncludeFile);
	}
}

void FConfigClassGenerator::GeneratorIncludeHeader(const FString &IncludeFile, FString &OutStr)
{
	OutStr += EndLinePrintf(TEXT("#include \"%s\""), *IncludeFile);
}
