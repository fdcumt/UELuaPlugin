#pragma once
#include "lua_tinker.h"

class LUAWRAPPER_API FLuaWrapper
{
public:
	FLuaWrapper();
	~FLuaWrapper();

public:
	void Restart();
	void Init();

public:
	lua_State* GetLuaState() { return m_LuaState; }

private:
	void InitLuaEnv();
	void InitGlobalTable();
	void CloseLuaEnv();
	void RegisterLuaLog();
	void RegisterAllClasses();
	void DoMainFile();

private:
	lua_State *m_LuaState;

};
