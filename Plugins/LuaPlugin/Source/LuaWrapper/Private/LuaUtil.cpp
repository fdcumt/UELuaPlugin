#include "LuaUtil.h"
#include "LuaWrapperDefine.h"

void FLuaUtil::RegisterModule(const luaL_Reg *ModuleFunctions, const char *ModuleName)
{

}

void FLuaUtil::AddModule(const char *ModuleName)
{
	lua_getglobal(g_LuaState, ModuleName); // 查找全局表ModuleName,放入栈顶
	if (lua_istable(g_LuaState, -1))
	{ // 栈顶元素是否是表
		lua_pop(g_LuaState, 1);
		return;
	}
	lua_pop(g_LuaState, 1); //把栈顶的空元素弹出

	// 创建元表moduleName,并放入全局表(l_gt)中
	lua_pushvalue(g_LuaState, LUA_GLOBALSINDEX); // 打开l_gt表,放入栈顶
	lua_pushstring(g_LuaState, ModuleName); // 把moduleName入栈,作为Key
	luaL_newmetatable(g_LuaState, ModuleName);// 创建元表,并压栈

	{// 设置表内容
		InitMetaFuncs();
		InitUserDefinedFuncs();
	}

	lua_rawset(g_LuaState, -3); // 将全局表ModuleName放入l_gt表中,并弹出两个参数
	lua_pop(g_LuaState, 1); // 把栈顶l_gt表弹出
}

void FLuaUtil::InitMetaFuncs()
{

}

void FLuaUtil::InitUserDefinedFuncs()
{

}

