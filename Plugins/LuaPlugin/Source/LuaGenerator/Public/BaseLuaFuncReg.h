#pragma once
#include "GeneratorDefine.h"

struct FVariableTypeInfo
{
public:
	void InitByUProperty(UProperty *pProperty);

public:
	int32 ArrayDim;
	FString OriginalType;
	FString DeclareType;
	FString VariableName;
	FString PropertyName;
	FString PureType;
	FString UsedSelfVarPrefix; 
	FString AssignValuePrefix;
	bool CanGenerateSetFunc;
	bool CanGenerateGetFunc;
	bool bSupportNow;
	NS_LuaGenerator::EVariableType eVariableType;

private:
	FString GetPureType(const FString &InType);
};

struct FExtraFuncMemberInfo
{
public:
	FString funcName;
	FString funcBody;
};

struct FExportFuncMemberInfo
{
public:
	static FExportFuncMemberInfo CreateFunctionMemberInfo(UFunction* InFunction);
	static bool CanExportFunction(UFunction* InFunction);

private:
	void InitByUFunction(UFunction* InFunction);

public:
	bool bStatic;
	bool bSupportNow;
	FString FunctionName;
	TArray<FVariableTypeInfo> FunctionParams;
	FVariableTypeInfo ReturnType;
};

struct FExportDataMemberInfo
{
public:
	static FExportDataMemberInfo CreateExportDataMemberInfo(UProperty *InProperty);
	static FExportDataMemberInfo GetCorrectDataMemberInfo(const FString &ClassName, const FExportDataMemberInfo &InDataMemberInfo);

public:
	bool CanExportDataMember(UProperty *InProperty);

private:
	void Init(UProperty *InProperty);

public:
	FVariableTypeInfo VariableInfo;
};

class FBaseFuncReg
{
public:
	void Init(const FString &ClassName);
	void AddDataMember(const FExportDataMemberInfo &InDataMemberInfo);
	void AddFunctionMember(const FExportFuncMemberInfo &ExportFuncInfo);
	void AddExtraFuncMember(const FExtraFuncMemberInfo&ExtraFuncMemberInfo);

public:
	FString GetFuncContents();
	FString GetRegLibContents();

private:
	FString GetLuaFuncMemberName(const FString &FuncName);
	FString GetLuaGetDataMemberName(const FString &VariableName);
	FString GetLuaSetDataMemberName(const FString &VariableName);

	FString GetExtraFuncContents();
	FString GetDataMemberContents();
	FString GetFuncMemberContents();

	FString GetExtraFuncContent(const FExtraFuncMemberInfo &InDataMemberInfo);
	FString GetLuaGetDataMemberFuncContent(const FExportDataMemberInfo &InDataMemberInfo);
	FString GetLuaSetDataMemberFuncContent(const FExportDataMemberInfo &InDataMemberInfo);

	FString GetLuaGetMutilDimDataMemberFuncContent(const FExportDataMemberInfo &InDataMemberInfo);
	FString GetLuaSetMutilDimDataMemberFuncContent(const FExportDataMemberInfo &InDataMemberInfo);
	
private:
	FString m_ClassName;
	TArray<FExtraFuncMemberInfo> m_ExtraFuncs;
	TMap<FString, FExportFuncMemberInfo> m_FunctionMembers;
	TMap<FString, FExportDataMemberInfo> m_DataMembers;
};


