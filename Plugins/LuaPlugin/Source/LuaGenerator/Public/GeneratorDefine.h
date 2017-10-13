#pragma once
#include "LuaConfigManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLuaGenerator, Log, All);

#define SafeDelete(pItem) if (pItem) { delete pItem; }
#define EndLinePrintf(FormatString, ...) FString::Printf(TEXT("%s\r\n"), *FString::Printf(FormatString, ##__VA_ARGS__ ))
#define GeneratorLog(LogVerbosity, FormatString, ...) UE_LOG(LogLuaGenerator, LogVerbosity, FormatString, ##__VA_ARGS__ )


#if 1
#define DebugLog(FormatString, ...) UE_LOG(LogLuaGenerator, Error, FormatString, ##__VA_ARGS__ )
#else
#define DebugLog(FormatString, ...)
#endif

extern class FScriptGeneratorManager *g_ScriptGeneratorManager;
extern class FLuaConfigManager *g_LuaConfigManager;

namespace NS_LuaGenerator
{
	enum E_GeneratorType
	{
		EUClass = 0,
		EUStruct = 1,
		EConfigClass = 2,
	};

	enum EVariableType
	{
		EBaseType, // int,uint16,uinit32,uint64,float,char*,bool
		EPoint,
		EMutilPoint, 
		EObjectBase,
		EFName,
		EText,
		EFString,
		EClass,
		ETArray,
		EWeakObject,
		EStruct,
		EByte,
		EEnum,
		ETSubclassOf,
		EMulticastDelegate,
		ETMap,
		ETSet,
		EVoid,
		EUnknow,
	};

	EVariableType ResolvePropertyType(UProperty *pProperty);
	int32 CalcStarNum(const FString &InStr);
	FString GetFuncParamPropertyType(UProperty *Property, uint32 PortFlags = 0);
	FString GetDataMemberPropertyType(UProperty *Property, uint32 PortFlags = 0);
	bool StringForwardContainSub(const FString &&SrcStr, const FString &&SubStr, int32 SrcIndex);
	bool StringBackContainSub(const FString &&SrcStr, const FString &&SubStr, int32 SrcTailIndex);
	bool CanExportProperty(UProperty *InProperty);
	bool CanExportFunction(UFunction *InFunction);
}


