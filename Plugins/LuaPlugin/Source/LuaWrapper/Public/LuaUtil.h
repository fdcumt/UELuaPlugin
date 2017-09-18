#pragma once
#include "lua_tinker.h"

class LUAWRAPPER_API FLuaUtil
{
public:
	static void RegisterModule(const luaL_Reg *ModuleFunctions, const char *ModuleName);


private:
	static void AddModule(const char *ModuleName);
	static void InitMetaFuncs(); // 初始化元表中的元操作
	static void InitUserDefinedFuncs(); // 初始化元表中的自定义操作
};
