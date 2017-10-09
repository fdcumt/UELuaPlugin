#pragma once

struct FVariableType
{
public:
	void Init(const FString &InOriginalType);

public:
	FString OriginalType;
};

struct FFunctionParamInfo
{
	TArray<FVariableType> ParamTypes;

public:
	int32 GetParamNum() const { return ParamTypes.Num(); }
};

struct FExportFunctionInfo
{
	FString FunctionName;
	FFunctionParamInfo FunctionParams;
	FVariableType ReturnType;
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


