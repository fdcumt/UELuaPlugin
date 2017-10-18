#pragma once
#include "LuaWrapperDefine.h"

class LUAWRAPPER_API FLuaWrapper
{
public:
	FLuaWrapper();
	~FLuaWrapper();

public:
	void Restart();
	void Init();

public:
	lua_State* GetLuaState() { return g_LuaState; }

private:
	void InitLuaEnv();
	void InitGlobalTable();
	void CloseLuaEnv();
	void RegisterLuaLog();
	void RegisterAllClasses();
	void DoMainFile();
};
