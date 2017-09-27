#pragma once
#include "Misc/FileHelper.h"
#include "UObjectIterator.h"
#include "Serialization/JsonSerializer.h"

struct FFunctionParam
{
public:
	FFunctionParam(int32 InIndex, const FString &InSrcName);

public:
	int32 Index;
	FString PureType;
	FString DeclareType;
	FString VariableName;
	FString UsedVariableStr;

private:
	bool bReadoutValue;
	TArray<FString> ParsedParam;

private:
	void ParseParam(const FString &InSrcName);
	void GenerateIsNeedReadout();
	void GeneratePureType();
	void GenerateDeclareType();
	void GenerateVariableName();
	void GenerateUsedVariableStr();
	bool ParamIsBaseType();
	bool ParamIsPointType();
};

struct FConfigFunction
{
public:
	FConfigFunction(const TSharedPtr<FJsonObject> &InJsonObj);

public:
	bool bStatic;
	FString Name;
	FString RetType;
	TArray<FString> ParamTypes;
	TArray<FFunctionParam> Params;

private:
	void ParseStaticType(const TSharedPtr<FJsonObject> &InJsonObj);
	void ParseFunctionName(const TSharedPtr<FJsonObject> &InJsonObj);
	void ParseRetType(const TSharedPtr<FJsonObject> &InJsonObj);
	void ParseParams(const TSharedPtr<FJsonObject> &InJsonObj);
};

struct FConfigClass
{
	FConfigClass(const TSharedPtr<FJsonObject> &InJson);
	FString Name;
	FString ParentName;
	TArray<FString> IncludeHeaders;
	TArray<FConfigFunction> Functions;

private:
	void ParseClassName(const TSharedPtr<FJsonObject> &InJsonClass);
	void ParseIncludeHeaders(const TSharedPtr<FJsonObject> &InJsonClass);
	void ParseParentName(const TSharedPtr<FJsonObject> &InJsonClass);
	void ParseFunctions(const TSharedPtr<FJsonObject> &InJsonClass);
};


