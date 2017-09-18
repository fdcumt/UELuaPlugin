#include "LuaUtil.h"
#include "LuaWrapperDefine.h"

void FLuaUtil::RegisterModule(const luaL_Reg *ModuleFunctions, const char *ModuleName)
{

}

void FLuaUtil::AddModule(const char *ModuleName)
{
	lua_getglobal(g_LuaState, ModuleName); // ����ȫ�ֱ�ModuleName,����ջ��
	if (lua_istable(g_LuaState, -1))
	{ // ջ��Ԫ���Ƿ��Ǳ�
		lua_pop(g_LuaState, 1);
		return;
	}
	lua_pop(g_LuaState, 1); //��ջ���Ŀ�Ԫ�ص���

	// ����Ԫ��moduleName,������ȫ�ֱ�(l_gt)��
	lua_pushvalue(g_LuaState, LUA_GLOBALSINDEX); // ��l_gt��,����ջ��
	lua_pushstring(g_LuaState, ModuleName); // ��moduleName��ջ,��ΪKey
	luaL_newmetatable(g_LuaState, ModuleName);// ����Ԫ��,��ѹջ

	{// ���ñ�����
		InitMetaFuncs();
		InitUserDefinedFuncs();
	}

	lua_rawset(g_LuaState, -3); // ��ȫ�ֱ�ModuleName����l_gt����,��������������
	lua_pop(g_LuaState, 1); // ��ջ��l_gt����
}

void FLuaUtil::InitMetaFuncs()
{

}

void FLuaUtil::InitUserDefinedFuncs()
{

}

