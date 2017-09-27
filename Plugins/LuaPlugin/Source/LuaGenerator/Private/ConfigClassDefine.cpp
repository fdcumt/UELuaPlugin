#include "ConfigClassDefine.h"
#include "GeneratorDefine.h"

FFunctionParam::FFunctionParam(int32 InIndex, const FString &InSrcName)
	: Index(InIndex)
	, bReadoutValue(false)
{
	DebugLog(TEXT("FFunctionParam"));
	ParseParam(InSrcName);
	DebugLog(TEXT("ParseParam"));
	GenerateIsNeedReadout();
	DebugLog(TEXT("GenerateIsNeedReadout"));

	GeneratePureType();
	DebugLog(TEXT("GeneratePureType"));
	GenerateDeclareType();
	DebugLog(TEXT("GenerateDeclareType"));
	GenerateVariableName();
	DebugLog(TEXT("GenerateVariableName"));
	GenerateUsedVariableStr();
	DebugLog(TEXT("GenerateUsedVariableStr"));
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
	while (BeginIndex <= EndIndex && ParsedParam[BeginIndex] != "const" && ParsedParam[BeginIndex] != "class")
	{
		++BeginIndex;
	}

	while (EndIndex >= BeginIndex && ParsedParam[EndIndex] != "*" && ParsedParam[EndIndex] != "&" && ParsedParam[EndIndex] != "const")
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

	while (BeginIndex <= EndIndex && ParsedParam[BeginIndex] != "const" && ParsedParam[BeginIndex] != "class")
	{
		++BeginIndex;
	}

	while (EndIndex >= BeginIndex && ParsedParam[EndIndex] != "&" && ParsedParam[EndIndex] != "const")
	{
		--EndIndex;
	}

	while (BeginIndex <= EndIndex)
	{
		PureType += ParsedParam[BeginIndex];
		++BeginIndex;
	}

	if (!PureType.IsEmpty() && bReadoutValue)
	{
		PureType += "*";
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
	DebugLog(TEXT("FConfigClass"));
	ParseClassName(InJsonObj);
	ParseIncludeHeaders(InJsonObj);
	ParseParentName(InJsonObj);
	ParseFunctions(InJsonObj);
}

void FConfigClass::ParseClassName(const TSharedPtr<FJsonObject> &InJsonClass)
{
	if (!InJsonClass->TryGetStringField("ClassName", Name))
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
			// parse header file
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

void FConfigClass::ParseParentName(const TSharedPtr<FJsonObject> &InJsonClass)
{
	if (!InJsonClass->TryGetStringField("ParentName", ParentName))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("try get class ParentName error!"));
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

FConfigFunction::FConfigFunction(const TSharedPtr<FJsonObject> &InJsonObj)
{
	DebugLog(TEXT("FConfigFunction"));
	ParseFunctionName(InJsonObj);
	ParseStaticType(InJsonObj);
	ParseRetType(InJsonObj);
	ParseParams(InJsonObj);
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
	if (!InJsonObj->TryGetStringField("FuncName", Name))
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
