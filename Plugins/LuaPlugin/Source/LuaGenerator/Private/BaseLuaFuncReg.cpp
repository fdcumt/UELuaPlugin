#include "BaseLuaFuncReg.h"
#include "GeneratorDefine.h"

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

		FunctionContents += EndLinePrintf(TEXT("\t//return type:%s"), *FunctionItem.ReturnType.OriginalType);

		for (int32 i=0; i<FunctionItem.FunctionParams.GetParamNum(); ++i)
		{
			FunctionContents += EndLinePrintf(TEXT("\t//Param type:%s"), *FunctionItem.FunctionParams.ParamTypes[i].OriginalType);
		}
		
		FunctionContents += EndLinePrintf(TEXT("\treturn 1;"));
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

void FVariableType::Init(const FString &InOriginalType)
{
	OriginalType = InOriginalType;
}
