#include "LuaWrapper.h"
#include "LuaUtil.h"
#include "LuaWrapperDefine.h"
#include "AllHeaders.h"
#include "LoadAllDefine.h"

FLuaWrapper::FLuaWrapper()
{
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

void FLuaWrapper::Init()
{
	InitLuaEnv();
	RegisterLuaLog();
	RegisterAllClasses();
	DoMainFile();
	LuaWrapperLog(Log, TEXT("FLuaWrapper init"));
}

void FLuaWrapper::InitLuaEnv()
{
	g_LuaState = m_LuaState = lua_open();
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
		lua_setmetatable(m_LuaState, -2); // table1[metatable] = table2
		lua_setfield(m_LuaState, LUA_REGISTRYINDEX, "_existuserdata"); // LUA_REGISTRYINDEX[_existuserdata] = table1
	}
}

void FLuaWrapper::CloseLuaEnv()
{
	lua_close(m_LuaState);
}

static int32 LuaUnrealLog(lua_State* LuaState)
{
	int ArgCount = lua_gettop(LuaState);
	FString Message;

	for (int ArgIndex = 1; ArgIndex <= ArgCount; ++ArgIndex)
	{
		if (lua_isstring(LuaState, ArgIndex))
		{
			Message += ANSI_TO_TCHAR(lua_tostring(LuaState, ArgIndex));
		}
	}

	LuaWrapperLog(Log, TEXT("%s"), *Message);

	return 0;
}

const luaL_Reg LuaPrint[] =
{
	{ "print", LuaUnrealLog },
	{ NULL, NULL }
};

void FLuaWrapper::RegisterLuaLog()
{
	FLuaUtil::RegisterClass(LuaPrint, "LuaPrint");
}

void FLuaWrapper::RegisterAllClasses()
{
	Def_LoadAll();
}

void FLuaWrapper::DoMainFile()
{
	FString luaDir = FPaths::ConvertRelativePathToFull(FPaths::GameDir() / TEXT("LuaSource"));
	FString LuaMainFile = luaDir / TEXT("main.lua");
	if (luaL_dofile(g_LuaState, TCHAR_TO_ANSI(*LuaMainFile)))
	{
		LuaWrapperLog(Fatal, TEXT("DoMainFile error %s!"),*lua_tostring(g_LuaState, -1));
	}
}
