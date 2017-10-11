#include "BaseLuaFuncReg.h"
#include "GeneratorDefine.h"
#include "ScriptGeneratorManager.h"
#include "CoreUObject.h"
using namespace NS_LuaGenerator;

void FBaseFuncReg::Init(const FString &ClassName)
{
	m_ClassName = ClassName;
}

void FBaseFuncReg::AddFunction(const FExportFunctionInfo &ExportFuncInfo)
{
	m_FunctionInfos.Add(ExportFuncInfo.FunctionName, ExportFuncInfo);
}

FString FBaseFuncReg::GetFunctionContents()
{
	FString FunctionContents;
	for (const auto &Item : m_FunctionInfos)
	{
		const FExportFunctionInfo &FunctionItem = Item.Value;

		FunctionContents += EndLinePrintf(TEXT(""));
		FunctionContents += EndLinePrintf(TEXT("static int32 %s(lua_State *InLuaState)"), *GetLuaFunctionName(FunctionItem.FunctionName));
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

		if (FunctionItem.ReturnType=="void")
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

FString FBaseFuncReg::GetRegLibContents()
{
	FString RegLibContents;

	RegLibContents += EndLinePrintf(TEXT(""));
	RegLibContents += EndLinePrintf(TEXT("static const luaL_Reg %s_Lib[] ="), *m_ClassName);
	RegLibContents += EndLinePrintf(TEXT("{"));

	for (const auto &Item : m_FunctionInfos)
	{
		const FExportFunctionInfo &FunctionItem = Item.Value;
		RegLibContents += EndLinePrintf(TEXT("\t{ \"%s\", %s },"), *FunctionItem.FunctionName, *GetLuaFunctionName(FunctionItem.FunctionName));
	}

	RegLibContents += EndLinePrintf(TEXT("\t{ NULL, NULL }"));
	RegLibContents += EndLinePrintf(TEXT("};"));

	return RegLibContents;
}

FString FBaseFuncReg::GetLuaFunctionName(const FString &FuncName)
{
	return m_ClassName + "_" + FuncName;
}

void FVariableTypeInfo::InitByUProperty(UProperty *pProperty)
{
	if (!pProperty)
	{
		OriginalType = "void";
		eVariableType = EVariableType::EVoid;
		DeclareType = OriginalType;
		bSupportNow = true;
		bDereference = false;
		return;
	}

	OriginalType = GetPropertyType(pProperty);
	eVariableType = ResolvePropertyType(pProperty);
	DeclareType = OriginalType;
	bDereference = false;

	switch (eVariableType)
	{
	case EVariableType::EBaseType:
	{
		bSupportNow = false;
		break;
	}
	case EVariableType::EPoint:
	{
		bSupportNow = true;
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
		DeclareType = OriginalType+"*";
		bDereference = true;
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
		bSupportNow = true;
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

void FExportFunctionInfo::InitByUFunction(UFunction* InFunction)
{
	if (!CanExportFunction(InFunction))
	{
		return;
	}

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

bool FExportFunctionInfo::CanExportFunction(UFunction* InFunction)
{
	if (InFunction->GetName().Contains("DEPRECATED") || InFunction->HasMetaData("DeprecatedFunction"))
		return false;

	if ( InFunction->GetName() == "ExecuteUbergraph")
		return false;

	if (InFunction->HasAnyFunctionFlags(FUNC_EditorOnly))
		return false;

	return true;
}
