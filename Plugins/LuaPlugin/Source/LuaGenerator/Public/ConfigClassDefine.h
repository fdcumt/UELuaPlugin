#pragma once
#include "Misc/FileHelper.h"
#include "UObjectIterator.h"
#include "Serialization/JsonSerializer.h"

class FConfigClass;

class FFunctionParam
{
public:
	FFunctionParam(int32 InIndex, const FString &InSrcName);

public:
	int32 Index;
	FString PureType;
	FString DeclareType;
	FString VariableName;
	FString UsedVariableStr;

public:
	FString GetPureType() const;
	FString GetDeclareType() const;
	FString GetVariableName() const;
	FString GetUsedVariableStr() const;

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

class FConfigFunction
{
public:
	FConfigFunction(const TSharedPtr<FJsonObject> &InJsonObj);

public:
	bool bStatic;
	bool IsNeedReturn;
	FString FunctionName;
	FString RetType;
	TArray<FString> ParamTypes;
	TArray<FFunctionParam> FuncParams;

public:
	FString GetFunctionName() const;
	FString GetFunctionBodyChunk(const FConfigClass &ConfigClass) const;
	FString GetInParamsStr() const ;
	FString GetPureReturnType() const ;

private:
	void ParseStaticType(const TSharedPtr<FJsonObject> &InJsonObj);
	void ParseFunctionName(const TSharedPtr<FJsonObject> &InJsonObj);
	void ParseRetType(const TSharedPtr<FJsonObject> &InJsonObj);
	void ParseParams(const TSharedPtr<FJsonObject> &InJsonObj);
	void ParseIsNeedReturn(const TSharedPtr<FJsonObject> &InJsonObj);
	void AdjustReturnType();
};

class FConfigVariable
{
public:
	FConfigVariable(const TSharedPtr<FJsonObject> &InJson);

public:
	bool bStatic;
	FString VariableType;
	FString DeclareType;
	FString DeclareType;
	FString VariableName;

public:
	FString GetVariableTrunck(const FString &ClassName) const ;

private:
	FString GenerateGetVariableTrunck(const FString &ClassName)const;
	FString GenerateSetVariableTrunck(const FString &ClassName)const;
};

class FConfigClass
{
public:
	FConfigClass(const TSharedPtr<FJsonObject> &InJson);
	
public:
	FString ClassName;
	TArray<FString> ParentNames;
	TArray<FString> IncludeHeaders;
	TArray<FConfigFunction> Functions;
	TArray<FConfigVariable> Variables;

public:
	FString GetIncludeFilesChunk();
	FString GetFunctionsChunk();
	FString GetRegLibChunk();
	FString GetClassName() const { return ClassName;}

public:
	FString GetRegLibName() const;
	FString GetLuaFunctionName(const FConfigFunction &ConfigFunctions) const;

private: // parse and init
	void ParseClassName(const TSharedPtr<FJsonObject> &InJsonClass);
	void ParseIncludeHeaders(const TSharedPtr<FJsonObject> &InJsonClass);
	void ParseParentNames(const TSharedPtr<FJsonObject> &InJsonClass);
	void ParseFunctions(const TSharedPtr<FJsonObject> &InJsonClass);
	void ParseVariables(const TSharedPtr<FJsonObject> &InJsonClass);

private: // generator chunk
	FString GetFunctionsChunk(const TArray<FConfigFunction> &ConfigFunctions);
	FString GetFunctionChunk(const FConfigFunction &ConfigFunctions);
	FString GetRegLibItemsChunk(const TArray<FConfigFunction> &ConfigFunctions);

private:
	TSet<FString> m_FunctionNames;
};


