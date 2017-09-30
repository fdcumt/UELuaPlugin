#include "ConfigClassDefine.h"
#include "GeneratorDefine.h"
#include "ConfigClassGenerator.h"
#include "ScriptGeneratorManager.h"

FFunctionParam::FFunctionParam(int32 InIndex, const FString &InSrcName)
	: Index(InIndex)
	, bReadoutValue(false)
{
	ParseParam(InSrcName);
	GeneratePureType();
	GenerateIsNeedReadout();
	GenerateDeclareType();
	GenerateVariableName();
	GenerateUsedVariableStr();
}

FString FFunctionParam::GetPureType() const
{
	return PureType;
}

FString FFunctionParam::GetDeclareType() const
{
	return DeclareType;
}

FString FFunctionParam::GetVariableName() const
{
	return VariableName;
}

FString FFunctionParam::GetUsedVariableStr() const
{
	return UsedVariableStr;
}

void FFunctionParam::ParseParam(const FString &InSrcName)
{
	// split the src string
	int32 BeginIndex = 0;
	for (int32 i = 0; i < InSrcName.Len(); ++i)
	{
		if (InSrcName[i] == ' ' || InSrcName[i] == '\t')
		{
			if (BeginIndex < i)
			{
				ParsedParam.Add(InSrcName.Mid(BeginIndex, i - BeginIndex));
			}
			BeginIndex = i + 1;
		}
		else if (InSrcName[i] == '*' || InSrcName[i] == '&')
		{
			if (BeginIndex < i)
			{
				ParsedParam.Add(InSrcName.Mid(BeginIndex, i - BeginIndex));
			}
			ParsedParam.Add(InSrcName.Mid(i, 1));
			BeginIndex = i + 1;
		}
	}

	if (BeginIndex < InSrcName.Len())
	{
		ParsedParam.Add(InSrcName.Mid(BeginIndex, InSrcName.Len() - BeginIndex));
	}
}

void FFunctionParam::GeneratePureType()
{
	// remove the const,*,& from class name
	int32 BeginIndex = 0;
	int32 EndIndex = ParsedParam.Num() - 1;
	while (BeginIndex <= EndIndex && (ParsedParam[BeginIndex] == "const" || ParsedParam[BeginIndex] == "class"))
	{
		++BeginIndex;
	}

	while (EndIndex >= BeginIndex && (ParsedParam[EndIndex] == "*" || ParsedParam[EndIndex] == "&" || ParsedParam[EndIndex] == "const"))
	{
		--EndIndex;
	}

	while (BeginIndex <= EndIndex)
	{
		PureType += ParsedParam[BeginIndex];
		++BeginIndex;
	}
}

void FFunctionParam::GenerateDeclareType()
{
	int32 BeginIndex = 0;
	int32 EndIndex = ParsedParam.Num() - 1;

	while (BeginIndex <= EndIndex && (ParsedParam[BeginIndex] == "const" || ParsedParam[BeginIndex] == "class"))
	{
		++BeginIndex;
	}

	while (EndIndex >= BeginIndex && (ParsedParam[EndIndex] == "&" || ParsedParam[EndIndex] == "const"))
	{
		--EndIndex;
	}

	while (BeginIndex <= EndIndex)
	{
		DeclareType += ParsedParam[BeginIndex];
		++BeginIndex;
	}

	if (!DeclareType.IsEmpty() && bReadoutValue)
	{
		DeclareType += "*";
	}
}

void FFunctionParam::GenerateVariableName()
{
	VariableName = FString::Printf(TEXT("Param_%d"), Index);
}

void FFunctionParam::GenerateUsedVariableStr()
{
	if (bReadoutValue)
	{
		UsedVariableStr = FString::Printf(TEXT("*%s"), *VariableName);
	}
	else
	{
		UsedVariableStr = VariableName;
	}
}

void FFunctionParam::GenerateIsNeedReadout()
{
	if (ParamIsBaseType())
	{
		bReadoutValue = false;
	}
	else if (ParamIsPointType())
	{
		bReadoutValue = false;
	}
	else
	{
		bReadoutValue = true;
	}
}

bool FFunctionParam::ParamIsBaseType()
{
	for (const FString &BaseType : NS_LuaGenerator::BaseTypes)
	{
		if (PureType == BaseType)
		{
			return true;
		}
	}

	return false;
}

bool FFunctionParam::ParamIsPointType()
{
	int32 Index = ParsedParam.Num()-1;
	while (Index >= 0)
	{
		if (ParsedParam[Index] == "*")
		{
			return true;
		}
		else if (ParsedParam[Index] == "const")
		{
			--Index;
		}
		else if (ParsedParam[Index] == "&")
		{
			--Index;
		}
		else
		{
			break;
		}
	}

	if (PureType.StartsWith("TSharedPtr<", ESearchCase::CaseSensitive))
	{
		return true;
	}

	return false;
}

FConfigClass::FConfigClass(const TSharedPtr<FJsonObject> &InJsonObj)
{
	ParseClassName(InJsonObj);
	ParseIncludeHeaders(InJsonObj);
	ParseParentNames(InJsonObj);
	ParseFunctions(InJsonObj);
}

FString FConfigClass::GetRegLibChunk()
{
	FString Ret;
	// generate reg head
	Ret += EndLinePrintf(TEXT(""));
	Ret += EndLinePrintf(TEXT("static const luaL_Reg %s[] ="), *GetRegLibName());
	Ret += EndLinePrintf(TEXT("{"));

	// reg body
	Ret += GetRegLibItemsChunk(Functions);

	// parent reg body
	TArray<FString> ParentNames = g_ScriptGeneratorManager->GetParentNames(ClassName);
	for (const FString& ParentName : ParentNames)
	{
		IScriptGenerator* pGenerator = g_ScriptGeneratorManager->GetGenerator(ParentName);
		if (pGenerator && pGenerator->GetType() == NS_LuaGenerator::EConfigClass)
		{
			FConfigClassGenerator *pConfigGenerator = (FConfigClassGenerator*)pGenerator;
			Ret += GetRegLibItemsChunk(pConfigGenerator->GetConfigFunctions());
		}
	}

	// reg tail
	Ret += EndLinePrintf(TEXT("\t{ NULL, NULL }"));
	Ret += EndLinePrintf(TEXT("};"));

	return Ret;
}

FString FConfigClass::GetIncludeFilesChunk()
{
	FString Ret;
	for (const FString &Item : IncludeHeaders)
	{
		Ret += EndLinePrintf(TEXT("#include\"%s\""), *Item);
	}
	return Ret;
}

FString FConfigClass::GetRegLibName() const
{
	return FString::Printf(TEXT("%s_Lib"), *ClassName);
}

FString FConfigClass::GetLuaFunctionName(const FConfigFunction &ConfigFunctions) const
{
	return FString::Printf(TEXT("%s_%s"), *ClassName, *ConfigFunctions.GetFunctionName());
}

void FConfigClass::ParseClassName(const TSharedPtr<FJsonObject> &InJsonClass)
{
	if (!InJsonClass->TryGetStringField("ClassName", ClassName))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("try get class name error!"));
	}
}

void FConfigClass::ParseIncludeHeaders(const TSharedPtr<FJsonObject> &InJsonClass)
{
	const TArray<TSharedPtr<FJsonValue>> *JsonHeaderFiles;
	if (InJsonClass->TryGetArrayField("HeaderFiles", JsonHeaderFiles))
	{
		for (const TSharedPtr<FJsonValue> &pJsonHeaderFile : *JsonHeaderFiles)
		{
			FString HeaderFile;
			if (pJsonHeaderFile->TryGetString(HeaderFile))
			{
				IncludeHeaders.Add(HeaderFile);
			}
			else
			{
				UE_LOG(LogLuaGenerator, Error, TEXT("try get class header file item error!"));
				return;
			}
		}
	}
	else
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("try get class HeaderFiles error!"));
	}
}

void FConfigClass::ParseParentNames(const TSharedPtr<FJsonObject> &InJsonClass)
{
	const TArray<TSharedPtr<FJsonValue>> *JsonFunctions;
	if (InJsonClass->TryGetArrayField("ParentNames", JsonFunctions))
	{
		for (const TSharedPtr<FJsonValue> &pFunctionInfoItem : *JsonFunctions)
		{
			FString parentName;
			if (pFunctionInfoItem->TryGetString(parentName))
			{
				ParentNames.Add(parentName);
			}
			else
			{
				UE_LOG(LogLuaGenerator, Error, TEXT("try get class ParentName error!"));
			}
		}
	}
	else
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("try get class ParentNames error!"));
	}
}

void FConfigClass::ParseFunctions(const TSharedPtr<FJsonObject> &InJsonClass)
{
	const TArray<TSharedPtr<FJsonValue>> *JsonFunctions;
	if (InJsonClass->TryGetArrayField("Functions", JsonFunctions))
	{
		for (const TSharedPtr<FJsonValue> &pFunctionInfoItem : *JsonFunctions)
		{
			Functions.Add(FConfigFunction(pFunctionInfoItem->AsObject()));
		}
	}
	else
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("try get class functions error!"));
	}
}

FString FConfigClass::GetFunctionsChunk(const TArray<FConfigFunction> &ConfigFunctions)
{
	FString RetChunk;
	for (const FConfigFunction&Item : ConfigFunctions)
	{
		RetChunk += GetFunctionChunk(Item);
	}
	return RetChunk;
}

FString FConfigClass::GetFunctionsChunk()
{
	FString FunctionsChunk;
	// own functions
	FunctionsChunk += GetFunctionsChunk(Functions);

	// parent class functions
	TArray<FString> ParentNames = g_ScriptGeneratorManager->GetParentNames(ClassName);
	for (const FString& ParentName : ParentNames )
	{
		IScriptGenerator* pGenerator = g_ScriptGeneratorManager->GetGenerator(ParentName);
		if (pGenerator && pGenerator->GetType()== NS_LuaGenerator::EConfigClass)
		{
			FConfigClassGenerator *pConfigGenerator = (FConfigClassGenerator*)pGenerator;
			FunctionsChunk += GetFunctionsChunk(pConfigGenerator->GetConfigFunctions());
		}
	}

	return FunctionsChunk;
}

FConfigFunction::FConfigFunction(const TSharedPtr<FJsonObject> &InJsonObj)
{
	ParseFunctionName(InJsonObj);
	ParseStaticType(InJsonObj);
	ParseRetType(InJsonObj);
	ParseParams(InJsonObj);
	ParseIsNeedReturn(InJsonObj);
}

FString FConfigFunction::GetFunctionName() const
{
	return FunctionName;
}

FString FConfigFunction::GetFunctionBodyChunk(const FConfigClass &ConfigClass) const
{
	FString Ret;
	FString CallFuncStr;

	for (int32 i=Params.Num()-1; i>=0; --i)
	{
		Ret += EndLinePrintf(TEXT("\t%s %s;"), *Params[i].GetDeclareType(), *Params[i].GetVariableName());
		Ret += EndLinePrintf(TEXT("\tFLuaUtil::Pop(InLuaState, FLuaClassType<%s>(%s, \"%s\"));"), *Params[i].GetDeclareType(), *Params[i].GetVariableName(), *Params[i].GetPureType());
	}

	CallFuncStr = FString::Printf(TEXT("%s(%s);"), *FunctionName, *GetInParamsStr());

	if (bStatic)
	{
		CallFuncStr = FString::Printf(TEXT("%s::%s"), *ConfigClass.GetClassName(), *CallFuncStr);
	}
	else
	{
		Ret += EndLinePrintf(TEXT("\t%s *pObj = FLuaUtil::TouserCppClassType<%s*>(InLuaState, \"%s\");"), *ConfigClass.GetClassName(), *ConfigClass.GetClassName(), *ConfigClass.GetClassName());
		CallFuncStr = FString::Printf(TEXT("pObj->%s"), *CallFuncStr);
	}

	if (IsNeedReturn)
	{
		Ret += EndLinePrintf(TEXT("\t%s RetValue = %s"), *RetType, *CallFuncStr);
		Ret += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(RetValue, \"%s\"));"), *RetType, *GetPureReturnType());
	}
	else
	{
		Ret += EndLinePrintf(TEXT("\t%s"), *CallFuncStr);
	}

	Ret += EndLinePrintf(TEXT("\treturn 1;"));
	return Ret;
}

FString FConfigFunction::GetInParamsStr() const 
{
	FString Ret;
	for (int32 i=0; i<Params.Num()-1; ++i)
	{
		Ret += Params[i].GetUsedVariableStr()+", ";
	}

	if (Params.Num()>0)
	{
		Ret += Params[Params.Num()-1].GetUsedVariableStr();
	}

	return Ret;
}

FString FConfigFunction::GetPureReturnType() const
{
	return FFunctionParam(-1, RetType).GetPureType();
}

void FConfigFunction::ParseStaticType(const TSharedPtr<FJsonObject> &InJsonObj)
{
	if (!InJsonObj->TryGetBoolField("IsStatic", bStatic))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("try get function IsStatic error!"));
	}
}

void FConfigFunction::ParseFunctionName(const TSharedPtr<FJsonObject> &InJsonObj)
{
	if (!InJsonObj->TryGetStringField("FuncName", FunctionName))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("try get Function name error!"));
	}
}

void FConfigFunction::ParseRetType(const TSharedPtr<FJsonObject> &InJsonObj)
{
	if (!InJsonObj->TryGetStringField("RetType", RetType))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("try get function RetType error!"));
	}

	AdjustReturnType();
}

void FConfigFunction::ParseParams(const TSharedPtr<FJsonObject> &InJsonObj)
{
	int32 index = 0;
	const TArray<TSharedPtr<FJsonValue>> *JsonFuncParamTypes;
	if (InJsonObj->TryGetArrayField("ParamTypes", JsonFuncParamTypes))
	{
		for (const TSharedPtr<FJsonValue> &pFuncParamTypeInfo : *JsonFuncParamTypes)
		{
			// parse function param
			FString StrParam;
			if (pFuncParamTypeInfo->TryGetString(StrParam))
			{
				Params.Add(FFunctionParam(index, StrParam));
			}
			else
			{
				UE_LOG(LogLuaGenerator, Error, TEXT("try get class function param item error!"));
			}

			++index;
		}
	}
	else
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("try get class function params error!"));
	}
}

void FConfigFunction::ParseIsNeedReturn(const TSharedPtr<FJsonObject> &InJsonObj)
{
	IsNeedReturn = !(RetType == "void");
}

void FConfigFunction::AdjustReturnType()
{
	FString StrReturnType = RetType;
	int32 BeginIndex = 0;
	int32 EndIndex = RetType.Len() - 1;
	while (BeginIndex <= EndIndex)
	{
		if (StrReturnType[BeginIndex] == '\t' || StrReturnType[BeginIndex] == ' ')
		{
			++BeginIndex;
		}
		else
		{
			break;
		}
	}

	while (EndIndex >= BeginIndex)
	{
		if (StrReturnType[EndIndex] == '\t' || StrReturnType[EndIndex] == ' ')
		{
			--EndIndex;
		}
		else
		{
			break;
		}
	}

	if (EndIndex >= BeginIndex)
	{
		RetType = StrReturnType.Mid(BeginIndex, EndIndex - BeginIndex + 1);
	}
	else
	{
		RetType.Empty();
	}
}

FString FConfigClass::GetRegLibItemsChunk(const TArray<FConfigFunction> &ConfigFunctions)
{
	FString Ret;
	for (const FConfigFunction &Item : ConfigFunctions)
	{
		Ret += EndLinePrintf(TEXT("\t{ \"%s\", %s },"), *Item.GetFunctionName(), *GetLuaFunctionName(Item));
	}
	return Ret;
}

FString FConfigClass::GetFunctionChunk(const FConfigFunction &ConfigFunction)
{
	FString Ret;
	FString LuaFunctionName = GetLuaFunctionName(ConfigFunction);
	Ret += EndLinePrintf(TEXT(""));
	Ret += EndLinePrintf(TEXT("static int32 %s(lua_State *InLuaState)"), *LuaFunctionName);
	Ret += EndLinePrintf(TEXT("{"));
	Ret += ConfigFunction.GetFunctionBodyChunk(*this);
	Ret += EndLinePrintf(TEXT("}"));

	if (m_FunctionNames.Contains(LuaFunctionName))
	{
		//UE_LOG(LogLuaGenerator, Error, TEXT("LuaFunctionName:%s has exist!"), *LuaFunctionName);
		return FString();
	}
	else
	{
		m_FunctionNames.Add(LuaFunctionName);
		return Ret;
	}
}
