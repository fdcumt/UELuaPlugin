#include "BaseLuaFuncReg.h"
#include "ScriptGeneratorManager.h"
#include "CoreUObject.h"
using namespace NS_LuaGenerator;

void FVariableTypeInfo::InitByUProperty(UProperty *pProperty)
{

	if (!pProperty)
	{
		OriginalType = "void";
		DeclareType = OriginalType;
		eVariableType = EVariableType::EVoid;
		bSupportNow = true;
		bNeedReturn = false;
		return;
	}

	OriginalType = GetPropertyType(pProperty);
	eVariableType = ResolvePropertyType(pProperty);
	ArrayDim = pProperty->ArrayDim;
	DeclareType = OriginalType;
	PureType = DeclareType;
	PropertyName = pProperty->GetName();
	VariableName = pProperty->GetName();
	UsedSelfVarPrefix = FString("");
	AssignValuePrefix = FString("");

	bNewReturn = false;
	CanGenerateSetFunc = true;
	CanGenerateGetFunc = true;
	bNeedReturn = OriginalType != "void";

	if (pProperty->PropertyFlags & CPF_DisableEditOnInstance)
	{
		CanGenerateSetFunc = false;
		CanGenerateGetFunc = true;
	}

	switch (eVariableType)
	{
	case EVariableType::EBaseType:
	{
		bSupportNow = true;
		break;
	}
	case EVariableType::EPoint:
	{
		DeclareType = OriginalType;
		PureType = GetPureType(OriginalType);
		bSupportNow = true;
		break;
	}
	case EVariableType::EMutilPoint:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::EObjectBase:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::EFName:
	{
		bSupportNow = true;
		break;
	}
	case EVariableType::EText:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::EFString:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::EClass:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::ETArray:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::EWeakObject:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::EStruct:
	{
		DeclareType = OriginalType + "*";
		PureType = OriginalType;
		UsedSelfVarPrefix = "*";
		AssignValuePrefix = "&";
		bNewReturn = true;
		bSupportNow = true;
		break;
	}
	case EVariableType::EByte:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::EEnum:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::ETSubclassOf:
	{
		int32 BeginIndex = strlen("TSubclassOf<");
		DeclareType = OriginalType.Mid(BeginIndex, OriginalType.Len() - BeginIndex - 1);
		DeclareType += "*";
		bSupportNow = false;
		break;
	}
	case EVariableType::EMulticastDelegate:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::ETMap:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::ETSet:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::EVoid:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::EUnknow:
	{
		bSupportNow = false;
		break;
	}
	}
}

FString FVariableTypeInfo::GetPureType(const FString &InType)
{
	// remove star suffix
	int32 EndIndex = InType.Len() - 1;
	while (EndIndex >= 0 && (InType[EndIndex] == ' ' || InType[EndIndex] == '*'))
	{
		--EndIndex;
	}

	return InType.Left(EndIndex + 1);
}

void FBaseFuncReg::Init(const FString &ClassName)
{
	m_ClassName = ClassName;
}

void FBaseFuncReg::AddExtraFuncMember(const FExtraFuncMemberInfo&ExtraFuncMemberInfo)
{
	m_ExtraFuncs.Add(ExtraFuncMemberInfo);
}

void FBaseFuncReg::AddFunctionMember(const FExportFuncMemberInfo &ExportFuncInfo)
{
	m_FunctionMembers.Add(ExportFuncInfo.FunctionName, ExportFuncInfo);
}

void FBaseFuncReg::AddDataMember(const FExportDataMemberInfo &InDataMemberInfo)
{
	m_DataMembers.Add(InDataMemberInfo.VariableInfo.VariableName, InDataMemberInfo);
}

FString FBaseFuncReg::GetFuncMemberContents()
{
	FString FunctionContents;
	for (const auto &Item : m_FunctionMembers)
	{
		FunctionContents += GetFuncMemberContent(Item.Value);
	}

	return FunctionContents;
}

FString FBaseFuncReg::GetFuncMemberContent(const FExportFuncMemberInfo &FunctionItem)
{
	FString FunctionContent;
	FunctionContent += EndLinePrintf(TEXT(""));
	FunctionContent += EndLinePrintf(TEXT("static int32 %s(lua_State *InLuaState)"), *GetLuaFuncMemberName(FunctionItem.FunctionName));
	FunctionContent += EndLinePrintf(TEXT("{"));

	if (FunctionItem.bSupportNow)
	{
		switch (FunctionItem.eFuncType)
		{
		case EExportFuncType::E_Normal:
		{
			FunctionContent += GetNomalFuncBody(FunctionItem);
			break;
		}
		case EExportFuncType::E_CallSuperFunc:
		{
			FunctionContent += GetCallSuperFuncBody(FunctionItem);
			break;
		}
		case EExportFuncType::E_MinimalAPI:
		{
			FunctionContent += GetMinmialAPIFuncBody(FunctionItem);
			break;
		}
		}
	}
	else
	{
		for (int32 i = 0; i < FunctionItem.FunctionParams.Num(); ++i)
		{
			FunctionContent += EndLinePrintf(TEXT("\t//OriginalTypeType:%s, DeclareType:%s"), *FunctionItem.FunctionParams[i].OriginalType, *FunctionItem.FunctionParams[i].DeclareType);
		}
		FunctionContent += EndLinePrintf(TEXT("\t//return OriginalTypeType:%s, DeclareType:%s"), *FunctionItem.ReturnType.OriginalType, *FunctionItem.ReturnType.DeclareType);
		FunctionContent += EndLinePrintf(TEXT("\treturn 0;"));
	}

	FunctionContent += EndLinePrintf(TEXT("}"));
	return FunctionContent;
}

FString FBaseFuncReg::GetDataMemberContents()
{
	FString RetContents;
	for (const auto &Item : m_DataMembers)
	{
		const FExportDataMemberInfo &DataMemberInfo = Item.Value;
		if (DataMemberInfo.VariableInfo.ArrayDim>1)
		{
			if (DataMemberInfo.VariableInfo.CanGenerateGetFunc)
			{
				RetContents += GetLuaGetMutilDimDataMemberFuncContent(DataMemberInfo);
			}

			if (DataMemberInfo.VariableInfo.CanGenerateSetFunc)
			{
				RetContents += GetLuaSetMutilDimDataMemberFuncContent(DataMemberInfo);
			}
		}
		else
		{
			if (DataMemberInfo.VariableInfo.CanGenerateGetFunc)
			{
				RetContents += GetLuaGetDataMemberFuncContent(DataMemberInfo);
			}

			if (DataMemberInfo.VariableInfo.CanGenerateSetFunc)
			{
				RetContents += GetLuaSetDataMemberFuncContent(DataMemberInfo);
			}
		}
	}
	return RetContents;
}

FString FBaseFuncReg::GetRegLibContents()
{
	FString RegLibContents;

	RegLibContents += EndLinePrintf(TEXT(""));
	RegLibContents += EndLinePrintf(TEXT("static const luaL_Reg %s_Lib[] ="), *m_ClassName);
	RegLibContents += EndLinePrintf(TEXT("{"));

	for (const auto &Item : m_FunctionMembers)
	{
		const FExportFuncMemberInfo &FunctionItem = Item.Value;
		RegLibContents += EndLinePrintf(TEXT("\t{ \"%s\", %s },"), *FunctionItem.FunctionName, *GetLuaFuncMemberName(FunctionItem.FunctionName));
	}

	for (const auto &Item : m_DataMembers)
	{
		const FExportDataMemberInfo &DataMember = Item.Value;
		if (DataMember.VariableInfo.CanGenerateGetFunc)
		{
			RegLibContents += EndLinePrintf(TEXT("\t{ \"Get_%s\", %s },"), *DataMember.VariableInfo.VariableName, *GetLuaGetDataMemberName(DataMember.VariableInfo.VariableName));
		}
		if (DataMember.VariableInfo.CanGenerateSetFunc)
		{
			RegLibContents += EndLinePrintf(TEXT("\t{ \"Set_%s\", %s },"), *DataMember.VariableInfo.VariableName, *GetLuaSetDataMemberName(DataMember.VariableInfo.VariableName));
		}
	}

	for (const FExtraFuncMemberInfo &Item : m_ExtraFuncs)
	{
		RegLibContents += EndLinePrintf(TEXT("\t{ \"%s\", %s },"), *Item.funcName, *GetLuaFuncMemberName(Item.funcName));
	}

	RegLibContents += EndLinePrintf(TEXT("\t{ NULL, NULL }"));
	RegLibContents += EndLinePrintf(TEXT("};"));

	return RegLibContents;
}

FString FBaseFuncReg::GetFuncContents()
{
	FString Ret;
	Ret += GetFuncMemberContents();
	Ret += GetDataMemberContents();
	Ret += GetExtraFuncContents();
	return Ret;
}

FString FBaseFuncReg::GetExtraFuncContents()
{
	FString Ret;
	for (const FExtraFuncMemberInfo&Item : m_ExtraFuncs)
	{
		Ret += GetExtraFuncContent(Item);
	}
	return Ret;
}

FString FBaseFuncReg::GetLuaFuncMemberName(const FString &FuncName)
{
	return m_ClassName + "_" + FuncName;
}

FString FBaseFuncReg::GetLuaGetDataMemberName(const FString &VariableName)
{
	return m_ClassName + "_Get_" + VariableName;
}

FString FBaseFuncReg::GetLuaSetDataMemberName(const FString &VariableName)
{
	return m_ClassName + "_Set_" + VariableName;
}

FString FBaseFuncReg::GetCallSuperFuncBody(const FExportFuncMemberInfo &FunctionItem)
{
	return EndLinePrintf(TEXT("\treturn %s_%s(InLuaState);"), *FunctionItem.SuperClassName, *FunctionItem.FunctionName);
}

FString FBaseFuncReg::GetNomalFuncBody(const FExportFuncMemberInfo &FunctionItem)
{
	FString FuncArgs;
	FString CallFunc;
	int32 luaStackIndex = 1;
	FString FuncBody;
	int32 FuncArgNum = FunctionItem.FunctionParams.Num();

	if (!FunctionItem.bStatic)
	{ // touser pObject
		FuncBody += EndLinePrintf(TEXT("\t%s *pObj = FLuaUtil::TouserData<%s*>(InLuaState, %d, \"%s\");"), *m_ClassName, *m_ClassName, luaStackIndex, *m_ClassName);
		++luaStackIndex;
	}

	for (int32 i = 0; i < FuncArgNum; ++i)
	{ // touser args
		const FVariableTypeInfo &VariableInfo = FunctionItem.FunctionParams[i];
		FuncBody += EndLinePrintf(TEXT("\t%s %s = FLuaUtil::TouserData<%s>(InLuaState, %d, \"%s\");"), *VariableInfo.DeclareType, *VariableInfo.VariableName, *VariableInfo.DeclareType, luaStackIndex, *VariableInfo.PureType);
		++luaStackIndex;
	}

	for (int32 i = 0; i < FuncArgNum - 1; ++i)
	{ // concat pre FuncArgNum-1 args 
		const FVariableTypeInfo &VariableInfo = FunctionItem.FunctionParams[i];
		FuncArgs += FString::Printf(TEXT("%s%s, "), *VariableInfo.UsedSelfVarPrefix, *VariableInfo.VariableName);
	}

	if (FuncArgNum > 0)
	{ // add the last arg to the FuncArgs
		const FVariableTypeInfo &VariableInfo = FunctionItem.FunctionParams[FuncArgNum - 1];
		FuncArgs += FString::Printf(TEXT("%s%s"), *VariableInfo.UsedSelfVarPrefix, *VariableInfo.VariableName);
	}

	if (FunctionItem.bStatic)
	{ // generate static be called function
		CallFunc += FString::Printf(TEXT("%s::%s(%s)"), *m_ClassName, *FunctionItem.FunctionName, *FuncArgs);
	}
	else
	{ // generate class member be called function
		CallFunc += FString::Printf(TEXT("pObj->%s(%s)"), *FunctionItem.FunctionName, *FuncArgs);
	}

	const FVariableTypeInfo &RetVarInfo = FunctionItem.ReturnType;
	if (RetVarInfo.OriginalType == "void")
	{ // call the function
		FuncBody += EndLinePrintf(TEXT("\t%s;"), *CallFunc);
	}
	else
	{ // call the function with return
		if (RetVarInfo.bNewReturn)
		{
			FuncBody += EndLinePrintf(TEXT("\t%s retVar = new %s();"), *RetVarInfo.DeclareType, *RetVarInfo.PureType);
			FuncBody += EndLinePrintf(TEXT("\t%sretVar = %s;"), *RetVarInfo.UsedSelfVarPrefix, *CallFunc);
		}
		else
		{
			FuncBody += EndLinePrintf(TEXT("\t%s retVar = %s;"), *RetVarInfo.DeclareType, *CallFunc);
		}

		FuncBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(retVar, \"%s\"));"), *RetVarInfo.DeclareType, *RetVarInfo.PureType);
	}

	if (RetVarInfo.bNewReturn)
	{
		FuncBody += EndLinePrintf(TEXT("\treturn 1;"));
	}
	else
	{
		FuncBody += EndLinePrintf(TEXT("\treturn 0;"));
	}

	return FuncBody;
}

FString FBaseFuncReg::GetMinmialAPIFuncBody(const FExportFuncMemberInfo &FunctionItem)
{
	FString FuncBody;
	int32 LuaStackIndex = 1;
	const FVariableTypeInfo &RetVarInfo = FunctionItem.ReturnType;

	// declare params
	FuncBody += EndLinePrintf(TEXT("\tstruct params"));
	FuncBody += EndLinePrintf(TEXT("\t{"));
	for (const FVariableTypeInfo &VarInfo :FunctionItem.FunctionParams)
	{ // add function params variable to args;
		FuncBody += EndLinePrintf(TEXT("\t\t%s %s;"), *VarInfo.OriginalType, *VarInfo.VariableName);
	}
	if (RetVarInfo.bNeedReturn)
	{ // add return variable to args;
		FuncBody += EndLinePrintf(TEXT("\t\t%s %s;"), *RetVarInfo.OriginalType, *RetVarInfo.VariableName);
	}
	FuncBody += EndLinePrintf(TEXT("\t}args;"));

	if (!FunctionItem.bStatic)
	{
		FuncBody += EndLinePrintf(TEXT("\t%s *pObj = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_ClassName, *m_ClassName, *m_ClassName);
		++LuaStackIndex;
	}
	
	for (const FVariableTypeInfo &VarInfo : FunctionItem.FunctionParams)
	{
		FuncBody += EndLinePrintf(TEXT("\targs.%s = %sFLuaUtil::TouserData<%s>(InLuaState, 1, \"%s\");"), *VarInfo.VariableName, *VarInfo.UsedSelfVarPrefix, *VarInfo.DeclareType, *VarInfo.PureType );
		++LuaStackIndex;
	}

	FuncBody += EndLinePrintf(TEXT("\tUFunction *Func = %s::StaticClass()->FindFunctionByName(FName(\"%s\"));"), *m_ClassName, *FunctionItem.FunctionName);
	if (FunctionItem.bStatic)
	{
		FuncBody += EndLinePrintf(TEXT("\tGetMutableDefault<%s>()->ProcessEvent(Func, &args);"), *m_ClassName);
	}
	else
	{
		FuncBody += EndLinePrintf(TEXT("\tpObj->ProcessEvent(Func, &args);"));
	}

	if (RetVarInfo.bNeedReturn)
	{
		if (RetVarInfo.bNewReturn)
		{
			FuncBody += EndLinePrintf(TEXT("\t%s NewRetValue = new %s();"), *RetVarInfo.DeclareType, *RetVarInfo.PureType);
			FuncBody += EndLinePrintf(TEXT("\t%sNewRetValue = args.%s;"), *RetVarInfo.UsedSelfVarPrefix, *RetVarInfo.VariableName);
			FuncBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(NewRetValue, \"%s\"));"), *RetVarInfo.DeclareType, *RetVarInfo.UsedSelfVarPrefix, *RetVarInfo.VariableName, *RetVarInfo.PureType);
		}
		else
		{
			FuncBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(%sargs.%s, \"%s\"));"), *RetVarInfo.DeclareType, *RetVarInfo.UsedSelfVarPrefix, *RetVarInfo.VariableName, *RetVarInfo.PureType);	
		}
		FuncBody += EndLinePrintf(TEXT("\treturn 1;"));
	}
	else
	{
		FuncBody += EndLinePrintf(TEXT("\treturn 0;"));
	}

	return FuncBody;
}

FString FBaseFuncReg::GetExtraFuncContent(const FExtraFuncMemberInfo &InDataMemberInfo)
{
	FString Ret;
	Ret += EndLinePrintf(TEXT(""));
	Ret += EndLinePrintf(TEXT("static int32 %s_%s(lua_State *InLuaState)"), *m_ClassName, *InDataMemberInfo.funcName);
	Ret += EndLinePrintf(TEXT("{"));
	Ret += InDataMemberInfo.funcBody;
	Ret += EndLinePrintf(TEXT("}"));
	return Ret;	
}

FString FBaseFuncReg::GetLuaGetDataMemberFuncContent(const FExportDataMemberInfo &InDataMemberInfo)
{
	FString RetContents;
	const FVariableTypeInfo &VariableInfo = InDataMemberInfo.VariableInfo;

	RetContents += EndLinePrintf(TEXT(""));
	RetContents += EndLinePrintf(TEXT("static int32 %s(lua_State *InLuaState)"), *GetLuaGetDataMemberName(VariableInfo.VariableName));
	RetContents += EndLinePrintf(TEXT("{"));
	
	if (VariableInfo.bSupportNow)
	{
		RetContents += EndLinePrintf(TEXT("\t%s *pObj = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_ClassName, *m_ClassName, *m_ClassName);
		RetContents += EndLinePrintf(TEXT("\t%s memberVariable = (%s)%spObj->%s;"), *VariableInfo.DeclareType, *VariableInfo.DeclareType, *VariableInfo.AssignValuePrefix, *VariableInfo.VariableName);
		RetContents += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(memberVariable, \"%s\"));"), *VariableInfo.DeclareType, *VariableInfo.PureType);
	}
	else
	{
		RetContents += EndLinePrintf(TEXT("\t//%s %s;"), *VariableInfo.DeclareType, *VariableInfo.VariableName);
	}

	RetContents += EndLinePrintf(TEXT("\treturn 1;"));
	RetContents += EndLinePrintf(TEXT("}"));

	return RetContents;
}

FString FBaseFuncReg::GetLuaSetDataMemberFuncContent(const FExportDataMemberInfo &InDataMemberInfo)
{
	FString RetContents;

	const FVariableTypeInfo &VariableInfo = InDataMemberInfo.VariableInfo;

	RetContents += EndLinePrintf(TEXT(""));
	RetContents += EndLinePrintf(TEXT("static int32 %s(lua_State *InLuaState)"), *GetLuaSetDataMemberName(VariableInfo.VariableName));
	RetContents += EndLinePrintf(TEXT("{"));

	if (VariableInfo.bSupportNow)
	{
		RetContents += EndLinePrintf(TEXT("\t%s *pObj = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_ClassName, *m_ClassName, *m_ClassName);
		RetContents += EndLinePrintf(TEXT("\t%s NewValue = FLuaUtil::TouserData<%s>(InLuaState, 2, \"%s\");"), *VariableInfo.DeclareType, *VariableInfo.DeclareType, *VariableInfo.PureType);
		RetContents += EndLinePrintf(TEXT("\tpObj->%s = %sNewValue;"), *VariableInfo.VariableName, *VariableInfo.UsedSelfVarPrefix);
	}
	else
	{
		RetContents += EndLinePrintf(TEXT("\t//%s %s;"), *VariableInfo.DeclareType, *VariableInfo.VariableName);
	}

	RetContents += EndLinePrintf(TEXT("\treturn 0;"));
	RetContents += EndLinePrintf(TEXT("}"));

	return RetContents;
}

FString FBaseFuncReg::GetLuaGetMutilDimDataMemberFuncContent(const FExportDataMemberInfo &InDataMemberInfo)
{
	FString RetContents;
	const FVariableTypeInfo &VariableInfo = InDataMemberInfo.VariableInfo;

	RetContents += EndLinePrintf(TEXT(""));
	RetContents += EndLinePrintf(TEXT("static int32 %s(lua_State *InLuaState)"), *GetLuaGetDataMemberName(VariableInfo.VariableName));
	RetContents += EndLinePrintf(TEXT("{"));

	if (VariableInfo.bSupportNow)
	{
		RetContents += EndLinePrintf(TEXT("\t%s *pObj = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_ClassName, *m_ClassName, *m_ClassName);
		RetContents += EndLinePrintf(TEXT("\tint32 Index = FLuaUtil::TouserData<int32>(InLuaState, 2, \"int32\");"));
		RetContents += EndLinePrintf(TEXT("\t%s memberVariable = (%s)%s(pObj->%s[Index]);"), *VariableInfo.DeclareType, *VariableInfo.DeclareType, *VariableInfo.AssignValuePrefix, *VariableInfo.VariableName);
		RetContents += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(memberVariable, \"%s\"));"), *VariableInfo.DeclareType, *VariableInfo.PureType);
	}
	else
	{
		RetContents += EndLinePrintf(TEXT("\t//%s %s;"), *VariableInfo.DeclareType, *VariableInfo.VariableName);
	}

	RetContents += EndLinePrintf(TEXT("\treturn 1;"));
	RetContents += EndLinePrintf(TEXT("}"));

	return RetContents;
}

FString FBaseFuncReg::GetLuaSetMutilDimDataMemberFuncContent(const FExportDataMemberInfo &InDataMemberInfo)
{
	FString RetContents;

	const FVariableTypeInfo &VariableInfo = InDataMemberInfo.VariableInfo;

	RetContents += EndLinePrintf(TEXT(""));
	RetContents += EndLinePrintf(TEXT("static int32 %s(lua_State *InLuaState)"), *GetLuaSetDataMemberName(VariableInfo.VariableName));
	RetContents += EndLinePrintf(TEXT("{"));

	if (VariableInfo.bSupportNow)
	{
		RetContents += EndLinePrintf(TEXT("\t%s *pObj = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_ClassName, *m_ClassName, *m_ClassName);
		RetContents += EndLinePrintf(TEXT("\tint32 Index = FLuaUtil::TouserData<int32>(InLuaState, 2, \"int32\");"));
		RetContents += EndLinePrintf(TEXT("\t%s NewValue = FLuaUtil::TouserData<%s>(InLuaState, 3, \"%s\");"), *VariableInfo.DeclareType, *VariableInfo.DeclareType, *VariableInfo.PureType);
		RetContents += EndLinePrintf(TEXT("\tpObj->%s[Index] = %sNewValue;"), *VariableInfo.VariableName, *VariableInfo.UsedSelfVarPrefix);
	}
	else
	{
		RetContents += EndLinePrintf(TEXT("\t//%s %s;"), *VariableInfo.DeclareType, *VariableInfo.VariableName);
	}

	RetContents += EndLinePrintf(TEXT("\treturn 0;"));
	RetContents += EndLinePrintf(TEXT("}"));

	return RetContents;
}

void FExportFuncMemberInfo::InitByUFunction(UClass *pClass, UFunction* InFunction)
{
	SuperClassName.Empty();
	UClass *pOwnerClass = InFunction->GetOwnerClass();
	if (pOwnerClass && pOwnerClass != pClass)
	{
		eFuncType = EExportFuncType::E_CallSuperFunc;
		SuperClassName = FString::Printf(TEXT("%s%s"), pOwnerClass->GetPrefixCPP(), *pOwnerClass->GetName());
	}
	else if (pClass->ClassFlags & CLASS_MinimalAPI)
	{
		eFuncType = EExportFuncType::E_MinimalAPI;
	}
	else
	{
		eFuncType = EExportFuncType::E_Normal;
	}

	InitByUFunctionInner(InFunction);
}

void FExportFuncMemberInfo::InitByUFunction(UStruct *pStruct, UFunction* InFunction)
{
	SuperClassName.Empty();
	UStruct *pOwnerStruct = InFunction->GetOwnerStruct();
	if ( pOwnerStruct && pOwnerStruct!=pStruct)
	{
		eFuncType = EExportFuncType::E_CallSuperFunc;
		SuperClassName = FString::Printf(TEXT("%s%s"), pOwnerStruct->GetPrefixCPP(), *pOwnerStruct->GetName());
	}
	else
	{
		eFuncType = EExportFuncType::E_Normal;
	}

	InitByUFunctionInner(InFunction);
}

void FExportFuncMemberInfo::InitByUFunctionInner(UFunction* InFunction)
{
	// init is static function
	bStatic = InFunction->HasAnyFunctionFlags(FUNC_Static);

	// init function name 
	FunctionName = InFunction->GetName();

	// init function return type
	UProperty *RetProperty = InFunction->GetReturnProperty();
	ReturnType.InitByUProperty(RetProperty);

	// init function param list
	for (TFieldIterator<UProperty> ParamIt(InFunction); ParamIt; ++ParamIt)
	{
		UProperty* Param = *ParamIt;
		if (!(Param->GetPropertyFlags() & CPF_ReturnParm))
		{
			FVariableTypeInfo FuncParam;
			FuncParam.InitByUProperty(Param);
			FunctionParams.Add(FuncParam);
		}
	}

	// can support now
	bSupportNow = true;
	for (const FVariableTypeInfo &VariableItem : FunctionParams)
	{
		if (!VariableItem.bSupportNow)
		{
			bSupportNow = false;
			break;
		}
	}
	if (ReturnType.bSupportNow == false)
	{
		bSupportNow = false;
	}
}

FExportFuncMemberInfo FExportFuncMemberInfo::CreateFunctionMemberInfo(UClass *pClass, UFunction* InFunction)
{
	FExportFuncMemberInfo functionInfo;
	functionInfo.InitByUFunction(pClass, InFunction);
	return functionInfo;
}

FExportFuncMemberInfo FExportFuncMemberInfo::CreateFunctionMemberInfo(UStruct *pStruct, UFunction* InFunction)
{
	FExportFuncMemberInfo functionInfo;
	functionInfo.InitByUFunction(pStruct, InFunction);
	return functionInfo;
}

bool FExportFuncMemberInfo::CanExportFunction(UFunction* InFunction)
{
	return true;
}

FExportFuncMemberInfo::FExportFuncMemberInfo()
	: bStatic(false)
	, bSupportNow(false)
{

}

FExportDataMemberInfo FExportDataMemberInfo::CreateExportDataMemberInfo(UProperty *InProperty)
{
	FExportDataMemberInfo dataMemberInfo;
	dataMemberInfo.Init(InProperty);
	return dataMemberInfo;
}

FExportDataMemberInfo FExportDataMemberInfo::GetCorrectDataMemberInfo(const FString &ClassName, const FExportDataMemberInfo &InDataMemberInfo)
{
	FExportDataMemberInfo memberInfo = InDataMemberInfo;
	FExportConfig *pExportConfig = g_LuaConfigManager->ExportConfig.Find(ClassName);

	if (g_LuaConfigManager->InvalidSetTypes.Contains(memberInfo.VariableInfo.OriginalType))
	{
		memberInfo.VariableInfo.CanGenerateSetFunc = false;
	}

	if (pExportConfig)
	{
		for (const FCorrectVariable &Item : pExportConfig->correctVariables)
		{
			if (memberInfo.VariableInfo.PureType == Item.VariableType && memberInfo.VariableInfo.PropertyName == Item.VariableName)
			{
				memberInfo.VariableInfo.PureType = Item.PureType;
				memberInfo.VariableInfo.DeclareType = Item.DeclareType;
				return memberInfo;
			}
		}
	}

	return memberInfo;
}

bool FExportDataMemberInfo::CanExportDataMember(UProperty *InProperty)
{
	return VariableInfo.bSupportNow;
}

void FExportDataMemberInfo::Init(UProperty *InProperty)
{
	VariableInfo.InitByUProperty(InProperty);
}
