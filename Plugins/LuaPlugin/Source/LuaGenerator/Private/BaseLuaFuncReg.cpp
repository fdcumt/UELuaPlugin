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
		TouserPushDeclareType = DeclareType;
		PureType = OriginalType;
		TouserPushPureType = PureType;
		eVariableType = EVariableType::EVoid;
		bSupportNow = true;
		bNeedReturn = false;
		bNeedExtraDeclarePushValue = false;
		return;
	}

	OriginalType = GetPropertyType(pProperty);
	eVariableType = ResolvePropertyType(pProperty);
	ArrayDim = pProperty->ArrayDim;
	bNeedExtraDeclarePushValue = false;
	CastType = "";

	DeclareType = OriginalType;
	TouserPushDeclareType = DeclareType;

	PureType = OriginalType;
	TouserPushPureType = PureType;

	PropertyName = pProperty->GetName();
	VariableName = PropertyName;

	UsedSelfVarPrefix = "";
	AssignPushVarPrefix = "";
	AssignPushVarSuffix = "";
	AssignValuePrefix = "";

	PointTValueDeclare = OriginalType + "*";
	PushPointTValuePrefix = "*";

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
		bSupportNow = true;
		PureType = GetPureType(OriginalType);
		TouserPushPureType = PureType;
		break;
	}
	case EVariableType::EFName:
	{
		bSupportNow = true;
		break;
	}
	case EVariableType::EText:
	{
		bSupportNow = true;
		break;
	}
	case EVariableType::EFString:
	{
		bSupportNow = true;
		break;
	}
	case EVariableType::EClass:
	{
		bSupportNow = true;
		break;
	}
	case EVariableType::EStruct:
	{
		bSupportNow = true;
		DeclareType = OriginalType + "*";
		TouserPushDeclareType = DeclareType;
		UsedSelfVarPrefix = "*";
		AssignValuePrefix = "&";
		PushPointTValuePrefix = "";
		bNewReturn = true;
		break;
	}
	case EVariableType::EByte:
	{
		bSupportNow = true;
		TouserPushDeclareType = "int32";
		TouserPushPureType = "int32";
		AssignPushVarPrefix = "(int32)";
		CastType = "("+DeclareType+")";
		bNeedExtraDeclarePushValue = true;
		break;
	}
	case EVariableType::EEnum:
	{
		bSupportNow = true;
		TouserPushDeclareType = "int32";
		TouserPushPureType = "int32";
		AssignPushVarPrefix = "(int32)";
		CastType = "(" + DeclareType + ")";
		bNeedExtraDeclarePushValue = true;
		break;
	}
	case EVariableType::EVarTArray:
	{
		bSupportNow = true;
		DeclareType = OriginalType + "*";
		TouserPushDeclareType = DeclareType;
		UsedSelfVarPrefix = "*";
		AssignValuePrefix = "&";
		PushPointTValuePrefix = "";
		bNewReturn = true;
		
		FString PlainType = GetPlainType(OriginalType);
		g_ScriptGeneratorManager->AddGeneratorProperty(PlainType, pProperty);
		break;
	}
	case EVariableType::EVarTSet:
	{
		bSupportNow = true;
		DeclareType = OriginalType + "*";
		TouserPushDeclareType = DeclareType;
		UsedSelfVarPrefix = "*";
		AssignValuePrefix = "&";
		PushPointTValuePrefix = "";
		bNewReturn = true;

		FString PlainType = GetPlainType(OriginalType);
		g_ScriptGeneratorManager->AddGeneratorProperty(PlainType, pProperty);
		break;
		break;
	}
	case EVariableType::EVarTMap:
	{
		bSupportNow = true;
		DeclareType = OriginalType + "*";
		TouserPushDeclareType = DeclareType;
		UsedSelfVarPrefix = "*";
		AssignValuePrefix = "&";
		PushPointTValuePrefix = "";
		bNewReturn = true;
		FString PlainType = GetPlainType(OriginalType);
		g_ScriptGeneratorManager->AddGeneratorProperty(PlainType, pProperty);
		break;
	}
	case EVariableType::ETSubclassOf:
	{
		bSupportNow = true;
		OriginalType = OriginalType.Mid(0, OriginalType.Len()-1);
		DeclareType = OriginalType;
		PureType = OriginalType;
		TouserPushDeclareType = "UClass*";
		TouserPushPureType = "UClass";
		CastType = "";
		UsedSelfVarPrefix = "";
		AssignPushVarPrefix = "*";
		AssignValuePrefix = "";
		PointTValueDeclare = DeclareType + "*";
		PushPointTValuePrefix ="";
		bNeedExtraDeclarePushValue = true;
		break;
	}
	case EVariableType::EWeakObject:
	{
		bSupportNow = true;
		DeclareType = OriginalType;
		PureType = OriginalType;
		TouserPushDeclareType = OriginalType.Mid(strlen("TWeakObjectPtr<"), OriginalType.Len()-strlen("TWeakObjectPtr<")-1);
		TouserPushPureType = TouserPushDeclareType;
		TouserPushDeclareType += "*";
		CastType = "";
		UsedSelfVarPrefix = "";
		AssignPushVarPrefix = "";
		AssignPushVarSuffix = ".Get()";
		AssignValuePrefix = "";
		PointTValueDeclare = DeclareType + "*";
		PushPointTValuePrefix = "*";
		bNeedExtraDeclarePushValue = true;
		break;
	}
	case EVariableType::EMulticastDelegate:
	{
		bSupportNow = false;
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
	if (ExtraFuncMemberInfo.funcName!="NoExport")
	{
		m_ExtraFuncs.Add(ExtraFuncMemberInfo);
	}
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
	if (!CanExportFunc(FunctionItem.FunctionName))
	{
		return FString("");
	}

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
			if (DataMemberInfo.VariableInfo.CanGenerateGetFunc && CanExportFunc(FString::Printf(TEXT("Get_%s"), *DataMemberInfo.VariableInfo.VariableName)))
			{
				RetContents += GetLuaGetMutilDimDataMemberFuncContent(DataMemberInfo);
			}

			if (DataMemberInfo.VariableInfo.CanGenerateSetFunc && CanExportFunc(FString::Printf(TEXT("Set_%s"), *DataMemberInfo.VariableInfo.VariableName)))
			{
				RetContents += GetLuaSetMutilDimDataMemberFuncContent(DataMemberInfo);
			}
		}
		else
		{
			if (DataMemberInfo.VariableInfo.CanGenerateGetFunc && CanExportFunc(FString::Printf(TEXT("Get_%s"), *DataMemberInfo.VariableInfo.VariableName)))
			{
				RetContents += GetLuaGetDataMemberFuncContent(DataMemberInfo);
			}

			if (DataMemberInfo.VariableInfo.CanGenerateSetFunc && CanExportFunc(FString::Printf(TEXT("Set_%s"), *DataMemberInfo.VariableInfo.VariableName)))
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
		if (CanExportFunc(FunctionItem.FunctionName))
		{
			RegLibContents += EndLinePrintf(TEXT("\t{ \"%s\", %s },"), *FunctionItem.FunctionName, *GetLuaFuncMemberName(FunctionItem.FunctionName));
		}
	}

	for (const auto &Item : m_DataMembers)
	{
		const FExportDataMemberInfo &DataMember = Item.Value;
		if (DataMember.VariableInfo.CanGenerateGetFunc && CanExportFunc(FString::Printf(TEXT("Get_%s"), *DataMember.VariableInfo.VariableName)))
		{
			RegLibContents += EndLinePrintf(TEXT("\t{ \"Get_%s\", %s },"), *DataMember.VariableInfo.VariableName, *GetLuaGetDataMemberName(DataMember.VariableInfo.VariableName));
		}
		if (DataMember.VariableInfo.CanGenerateSetFunc && CanExportFunc(FString::Printf(TEXT("Set_%s"), *DataMember.VariableInfo.VariableName)))
		{
			RegLibContents += EndLinePrintf(TEXT("\t{ \"Set_%s\", %s },"), *DataMember.VariableInfo.VariableName, *GetLuaSetDataMemberName(DataMember.VariableInfo.VariableName));
		}
	}

	for (const FExtraFuncMemberInfo &Item : m_ExtraFuncs)
	{
		if (CanExportFunc(*Item.funcName))
		{
			RegLibContents += EndLinePrintf(TEXT("\t{ \"%s\", %s },"), *Item.funcName, *GetLuaFuncMemberName(Item.funcName));
		}
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

bool FBaseFuncReg::CanExportFunc(const FString &FuncName)
{
	FExportConfig *pExportConfig = g_LuaConfigManager->ExportConfig.Find(m_ClassName);
	if (pExportConfig && pExportConfig->NotExportFunctions.Contains(FuncName))
	{
		return false;
	}
	else
	{
		return true;
	}
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
		FuncBody += EndLinePrintf(TEXT("\t%s %s = %sFLuaUtil::TouserData<%s>(InLuaState, %d, \"%s\");"), *VariableInfo.DeclareType, *VariableInfo.VariableName, *VariableInfo.CastType, *VariableInfo.TouserPushDeclareType, luaStackIndex, *VariableInfo.TouserPushPureType);
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

		if (RetVarInfo.bNeedExtraDeclarePushValue)
		{
			FuncBody += EndLinePrintf(TEXT("\t%s PushNewValue = %sretVar%s;"), *RetVarInfo.TouserPushDeclareType, *RetVarInfo.AssignPushVarPrefix, *RetVarInfo.AssignPushVarSuffix);
			FuncBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(PushNewValue, \"%s\"));"), *RetVarInfo.TouserPushDeclareType, *RetVarInfo.TouserPushPureType);
		}
		else
		{
			FuncBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(retVar, \"%s\"));"), *RetVarInfo.TouserPushDeclareType, *RetVarInfo.TouserPushPureType);
		}
	}

	if (RetVarInfo.bNeedReturn)
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
		FuncBody += EndLinePrintf(TEXT("\targs.%s = %s%sFLuaUtil::TouserData<%s>(InLuaState, 1, \"%s\");"), *VarInfo.VariableName, *VarInfo.UsedSelfVarPrefix, *VarInfo.CastType, *VarInfo.TouserPushDeclareType, *VarInfo.TouserPushPureType );
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
			if (RetVarInfo.bNeedExtraDeclarePushValue)
			{
				FuncBody += EndLinePrintf(TEXT("\t%s NewPushValue = %sNewRetValue%s;"), *RetVarInfo.TouserPushDeclareType, *RetVarInfo.AssignPushVarPrefix, *RetVarInfo.AssignPushVarSuffix);
				FuncBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(NewRetValue, \"%s\"));"), *RetVarInfo.TouserPushDeclareType, *RetVarInfo.TouserPushPureType);
			}
			else
			{
				FuncBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(NewRetValue, \"%s\"));"), *RetVarInfo.TouserPushDeclareType, *RetVarInfo.TouserPushPureType);
			}
		}
		else
		{
			if (RetVarInfo.bNeedExtraDeclarePushValue)
			{
				FuncBody += EndLinePrintf(TEXT("\t%s NewPushValue = %sargs.%s%s;"), *RetVarInfo.TouserPushDeclareType, *RetVarInfo.AssignPushVarPrefix, *RetVarInfo.VariableName, *RetVarInfo.AssignPushVarSuffix);
				FuncBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(NewPushValue, \"%s\"));"), *RetVarInfo.TouserPushDeclareType, *RetVarInfo.TouserPushPureType);
			}
			else
			{
				FuncBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(args.%s, \"%s\"));"), *RetVarInfo.TouserPushDeclareType, *RetVarInfo.VariableName,*RetVarInfo.TouserPushPureType);
			}
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
	if (!CanExportFunc(InDataMemberInfo.funcName))
	{
		return FString("");
	}

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
		if (VariableInfo.bNeedExtraDeclarePushValue)
		{
			RetContents += EndLinePrintf(TEXT("\t%s memberVariable1 = (%s)%spObj->%s;"), *VariableInfo.DeclareType, *VariableInfo.DeclareType, *VariableInfo.AssignValuePrefix, *VariableInfo.VariableName);
			RetContents += EndLinePrintf(TEXT("\t%s memberVariable = %smemberVariable1%s;"), *VariableInfo.TouserPushDeclareType, *VariableInfo.AssignPushVarPrefix, *VariableInfo.AssignPushVarSuffix);
		}
		else
		{
			RetContents += EndLinePrintf(TEXT("\t%s memberVariable = (%s)%spObj->%s;"), *VariableInfo.DeclareType, *VariableInfo.DeclareType, *VariableInfo.AssignValuePrefix, *VariableInfo.VariableName);
		}
		RetContents += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(memberVariable, \"%s\"));"), *VariableInfo.TouserPushDeclareType, *VariableInfo.TouserPushPureType);
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
		RetContents += EndLinePrintf(TEXT("\t%s NewValue = %sFLuaUtil::TouserData<%s>(InLuaState, 2, \"%s\");"), *VariableInfo.DeclareType, *VariableInfo.CastType, *VariableInfo.TouserPushDeclareType, *VariableInfo.TouserPushPureType);
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
		if (VariableInfo.bNeedExtraDeclarePushValue)
		{
			RetContents += EndLinePrintf(TEXT("\t%s memberVariable1 = (%s)%s(pObj->%s[Index]);"), *VariableInfo.DeclareType, *VariableInfo.DeclareType, *VariableInfo.AssignValuePrefix, *VariableInfo.VariableName);
			RetContents += EndLinePrintf(TEXT("\t%s memberVariable = %smemberVariable1%s;"), *VariableInfo.TouserPushDeclareType, *VariableInfo.AssignPushVarPrefix, *VariableInfo.AssignPushVarSuffix);
		}
		else
		{
			RetContents += EndLinePrintf(TEXT("\t%s memberVariable = (%s)%s(pObj->%s[Index]);"), *VariableInfo.DeclareType, *VariableInfo.DeclareType, *VariableInfo.AssignValuePrefix, *VariableInfo.VariableName);
		}
		RetContents += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(memberVariable, \"%s\"));"), *VariableInfo.TouserPushDeclareType, *VariableInfo.TouserPushPureType);
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
		RetContents += EndLinePrintf(TEXT("\t%s NewValue = %sFLuaUtil::TouserData<%s>(InLuaState, 3, \"%s\");"), *VariableInfo.DeclareType, *VariableInfo.CastType, *VariableInfo.TouserPushDeclareType, *VariableInfo.TouserPushPureType);
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
				memberInfo.VariableInfo.TouserPushPureType = Item.PureType;
				memberInfo.VariableInfo.DeclareType = Item.DeclareType;
				memberInfo.VariableInfo.TouserPushDeclareType = Item.DeclareType;
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
