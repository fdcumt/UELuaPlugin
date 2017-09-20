#include "LuaWrapper.h"
#include "LuaWrapperDefine.h"

FLuaWrapper::FLuaWrapper()
{
	LuaWrapperLog(Log, TEXT("FLuaWrapper"));
	InitLuaEnv();
}

FLuaWrapper::~FLuaWrapper()
{

}

void FLuaWrapper::Restart()
{
	CloseLuaEnv();
	InitLuaEnv();
	LuaWrapperLog(Log, TEXT("Restart"));
}

void FLuaWrapper::InitLuaEnv()
{
	g_LuaState = m_LuaState = lua_open();
	luaopen_base(m_LuaState);
	luaL_openlibs(m_LuaState);
	InitGlobalTable();
}

void FLuaWrapper::InitGlobalTable()
{
	{ 
		lua_newtable(m_LuaState); // table1
		lua_newtable(m_LuaState); // table2
		lua_pushstring(m_LuaState, "v");
		lua_setfield(m_LuaState, -2, "__mode"); //  set table2 value is weak 
		lua_setmetatable(L, -2); // table1[metatable] = table2
		lua_setfield(L, LUA_REGISTRYINDEX, "_existuserdata"); // LUA_REGISTRYINDEX[_existuserdata] = table1
	}
}

void FLuaWrapper::CloseLuaEnv()
{
	lua_close(m_LuaState);
}
