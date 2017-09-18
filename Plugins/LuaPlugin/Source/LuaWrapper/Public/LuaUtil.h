#pragma once
#include "lua_tinker.h"

class LUAWRAPPER_API FLuaUtil
{
public:
	static void RegisterModule(const luaL_Reg *ModuleFunctions, const char *ModuleName);


private:
	static void AddModule(const char *ModuleName);
	static void InitMetaFuncs(); // ��ʼ��Ԫ���е�Ԫ����
	static void InitUserDefinedFuncs(); // ��ʼ��Ԫ���е��Զ������
};
