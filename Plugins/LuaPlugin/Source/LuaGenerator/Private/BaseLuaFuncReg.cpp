#include "BaseLuaFuncReg.h"
#include "GeneratorDefine.h"
#include "ScriptGeneratorManager.h"
#include "CoreUObject.h"
using namespace NS_LuaGenerator;

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
		const FExportFuncMemberInfo &FunctionItem = Item.Value;

		FunctionContents += EndLinePrintf(TEXT(""));
		FunctionContents += EndLinePrintf(TEXT("static int32 %s(lua_State *InLuaState)"), *GetLuaFuncMemberName(FunctionItem.FunctionName));
		FunctionContents += EndLinePrintf(TEXT("{"));

		if (!FunctionItem.bSupportNow)
		{
			for (int32 i = 0; i < FunctionItem.FunctionParams.Num(); ++i)
			{
				FunctionContents += EndLinePrintf(TEXT("\t//Param type:%s"), *FunctionItem.FunctionParams[i].OriginalType);
			}
			FunctionContents += EndLinePrintf(TEXT("\t//return type:%s"), *FunctionItem.ReturnType.OriginalType);
		}
		else
		{
			for (int32 i = 0; i < FunctionItem.FunctionParams.Num(); ++i)
			{
				FunctionContents += EndLinePrintf(TEXT("\t//OriginalTypeType:%s, DeclareType:%s"), *FunctionItem.FunctionParams[i].OriginalType, *FunctionItem.FunctionParams[i].DeclareType);
			}
			FunctionContents += EndLinePrintf(TEXT("\t//return OriginalTypeType:%s, DeclareType:%s"), *FunctionItem.ReturnType.OriginalType, *FunctionItem.ReturnType.DeclareType);
		}

		if (FunctionItem.ReturnType.OriginalType=="void")
		{
			FunctionContents += EndLinePrintf(TEXT("\treturn 0;"));
		}
		else
		{
			FunctionContents += EndLinePrintf(TEXT("\treturn 1;"));
		}
		
		FunctionContents += EndLinePrintf(TEXT("}"));
	}

	return FunctionContents;
}

FString FBaseFuncReg::GetDataMemberContents()
{
	FString RetContents;
	for (const auto &Item : m_DataMembers)
	{
		const FExportDataMemberInfo &DataMemberInfo = Item.Value;
		RetContents += GetLuaGetDataMemberFuncContent(DataMemberInfo);
		RetContents += GetLuaSetDataMemberFuncContent(DataMemberInfo);
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
		RegLibContents += EndLinePrintf(TEXT("\t{ \"Get_%s\", %s },"), *DataMember.VariableInfo.VariableName, *GetLuaGetDataMemberName(DataMember.VariableInfo.VariableName));
		RegLibContents += EndLinePrintf(TEXT("\t{ \"Set_%s\", %s },"), *DataMember.VariableInfo.VariableName, *GetLuaSetDataMemberName(DataMember.VariableInfo.VariableName));
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
	if (!VariableInfo.bSupportNow)
	{
		RetContents += EndLinePrintf(TEXT("\t//%s %s;"), *VariableInfo.DeclareType, *VariableInfo.VariableName);
	}
	else
	{
		RetContents += EndLinePrintf(TEXT("\t%s *pObj = FLuaUtil::TouserCppClassType<%s*>(InLuaState, \"%s\");"), *m_ClassName, *m_ClassName, *m_ClassName);
		RetContents += EndLinePrintf(TEXT("\t%s memberVariable = (%s)%spObj->%s;"), *VariableInfo.DeclareType, *VariableInfo.DeclareType, *VariableInfo.AssignValuePrefix, *VariableInfo.VariableName);
		RetContents += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(memberVariable, \"%s\"));"), *VariableInfo.DeclareType, *VariableInfo.PureType);
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

	if (!VariableInfo.bSupportNow)
	{
		RetContents += EndLinePrintf(TEXT("\t//%s %s;"), *VariableInfo.DeclareType, *VariableInfo.VariableName);
	}
	else
	{
		RetContents += EndLinePrintf(TEXT("\t%s NewValue;"), *VariableInfo.DeclareType);
		RetContents += EndLinePrintf(TEXT("\tFLuaUtil::Pop(InLuaState, FLuaClassType<%s>(NewValue, \"%s\"));"), *VariableInfo.DeclareType, *VariableInfo.PureType);
		RetContents += EndLinePrintf(TEXT("\t%s *pObj = FLuaUtil::TouserCppClassType<%s*>(InLuaState, \"%s\");"), *m_ClassName, *m_ClassName, *m_ClassName);
		RetContents += EndLinePrintf(TEXT("\tpObj->%s = %sNewValue;"), *VariableInfo.VariableName, *VariableInfo.UsedSelfVarPrefix);
	}

	RetContents += EndLinePrintf(TEXT("\treturn 0;"));
	RetContents += EndLinePrintf(TEXT("}"));

	return RetContents;
}

void FVariableTypeInfo::InitByUProperty(UProperty *pProperty)
{
	if (!pProperty)
	{
		OriginalType = "void";
		DeclareType = OriginalType;
		eVariableType = EVariableType::EVoid;
		bSupportNow = true;
		return;
	}

	OriginalType = GetFuncParamPropertyType(pProperty);
	eVariableType = ResolvePropertyType(pProperty);
	DeclareType = OriginalType;
	PureType = DeclareType;
	VariableName = pProperty->GetName();
	UsedSelfVarPrefix = FString("");
	AssignValuePrefix = FString("");

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
		bSupportNow = false;
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
	int32 EndIndex = InType.Len()-1;
	while (EndIndex>=0 && (InType[EndIndex]==' '||InType[EndIndex]=='*'))
	{
		--EndIndex;
	}

	return InType.Left(EndIndex + 1);
}

void FExportFuncMemberInfo::InitByUFunction(UFunction* InFunction)
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

FExportFuncMemberInfo FExportFuncMemberInfo::CreateFunctionMemberInfo(UFunction* InFunction)
{
	FExportFuncMemberInfo functionInfo;
	functionInfo.InitByUFunction(InFunction);
	return functionInfo;
}

bool FExportFuncMemberInfo::CanExportFunction(UFunction* InFunction)
{
	return true;
}

FExportDataMemberInfo FExportDataMemberInfo::CreateExportDataMemberInfo(UProperty *InProperty)
{
	FExportDataMemberInfo dataMemberInfo;
	dataMemberInfo.Init(InProperty);
	return dataMemberInfo;
}

bool FExportDataMemberInfo::CanExportDataMember(UProperty *InProperty)
{
	return VariableInfo.bSupportNow;
}

void FExportDataMemberInfo::Init(UProperty *InProperty)
{
	VariableInfo.InitByUProperty(InProperty);
}
