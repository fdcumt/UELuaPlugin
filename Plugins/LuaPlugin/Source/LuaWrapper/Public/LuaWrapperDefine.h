#pragma once
#include "lua_tinker.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLua, Log, All);

#define LuaWrapperLog(LogVerbosity, FormatString, ...) UE_LOG(LogLua, LogVerbosity, FormatString, ##__VA_ARGS__ )

LUAWRAPPER_API extern struct lua_State  *g_LuaState;
extern class FLuaWrapper *g_LuaWrapper;

