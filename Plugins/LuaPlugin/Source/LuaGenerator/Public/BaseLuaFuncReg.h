#pragma once
#include "GeneratorDefine.h"

struct FVariableTypeInfo
{
public:
	void InitByUProperty(UProperty *pProperty);

public:
	int32 ArrayDim;
	FString PureType;
	FString OriginalType;
	FString DeclareType;
	FString CastType;
	FString TouserPushDeclareType;
	FString TouserPushPureType;
	FString UsedSelfVarPrefix;
	FString AssignPushVarPrefix;
	FString AssignPushVarSuffix;
	FString AssignValuePrefix;
	FString VariableName;
	FString PropertyName;
	FString PointTValueDeclare;
	FString PushPointTValuePrefix;
	bool CanGenerateSetFunc;
	bool CanGenerateGetFunc;
	bool bSupportNow;
	bool bNeedReturn;
	bool bNeedExtraDeclarePushValue;
	bool bNewReturn; // for return variable
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

enum EExportFuncType
{
	E_Normal,
	E_CallSuperFunc,
	E_MinimalAPI,
};

struct FExportFuncMemberInfo
{
public:
	static FExportFuncMemberInfo CreateFunctionMemberInfo(UClass *pClass, UFunction* InFunction);
	static FExportFuncMemberInfo CreateFunctionMemberInfo(UStruct *pStruct, UFunction* InFunction);
	static bool CanExportFunction(UFunction* InFunction);

public:
	FExportFuncMemberInfo();

private:
	void InitByUFunction(UClass *pClass, UFunction* InFunction);
	void InitByUFunction(UStruct *pStruct, UFunction* InFunction);
	void InitByUFunctionInner(UFunction* InFunction);

public:
	bool bStatic;
	bool bSupportNow;
	EExportFuncType eFuncType;
	FString FunctionName;
	FString SuperClassName;
	FVariableTypeInfo ReturnType;
	TArray<FVariableTypeInfo> FunctionParams;
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
	bool CanExportFunc(const FString &FuncName);

	FString GetLuaFuncMemberName(const FString &FuncName);
	FString GetLuaGetDataMemberName(const FString &VariableName);
	FString GetLuaSetDataMemberName(const FString &VariableName);

	FString GetExtraFuncContents();
	FString GetDataMemberContents();
	FString GetFuncMemberContents();

	FString GetFuncMemberContent(const FExportFuncMemberInfo &FunctionItem);
	FString GetCallSuperFuncBody(const FExportFuncMemberInfo &FunctionItem);
	FString GetNomalFuncBody(const FExportFuncMemberInfo &FunctionItem);
	FString GetMinmialAPIFuncBody(const FExportFuncMemberInfo &FunctionItem);

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


