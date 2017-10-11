#pragma once
#include "GeneratorDefine.h"

struct FVariableTypeInfo
{
public:
	void InitByUProperty(UProperty *pProperty);

public:
	FString OriginalType;
	FString DeclareType;
	bool bDereference;
	bool bSupportNow;
	NS_LuaGenerator::EVariableType eVariableType;
};

struct FExportFunctionInfo
{
public:
	void InitByUFunction(UFunction* InFunction);

public:
	bool bStatic;
	bool bSupportNow;
	FString FunctionName;
	TArray<FVariableTypeInfo> FunctionParams;
	FVariableTypeInfo ReturnType;

private:
	bool CanExportFunction(UFunction* InFunction);
};

class FBaseFuncReg
{
public:
	void Init(const FString &ClassName);
	void AddFunction(const FExportFunctionInfo &ExportFuncInfo);
	FString GetFunctionContents();
	FString GetRegLibContents();

private:
	FString GetLuaFunctionName(const FString &FuncName);
	
private:
	FString m_ClassName;
	TMap<FString, FExportFunctionInfo> m_FunctionInfos;
};


