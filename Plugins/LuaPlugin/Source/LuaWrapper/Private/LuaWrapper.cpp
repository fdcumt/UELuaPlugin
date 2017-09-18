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
}

void FLuaWrapper::CloseLuaEnv()
{
	lua_close(m_LuaState);
}
