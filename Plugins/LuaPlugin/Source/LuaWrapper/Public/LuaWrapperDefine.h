#pragma once
#include "lua_tinker.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLuaWrapper, Log, All);

#define LuaWrapperLog(LogVerbosity, FormatString, ...) UE_LOG(LogLuaWrapper, LogVerbosity, FormatString, ##__VA_ARGS__ )

extern struct lua_State  *g_LuaState;
extern class FLuaWrapper *g_LuaWrapper;

