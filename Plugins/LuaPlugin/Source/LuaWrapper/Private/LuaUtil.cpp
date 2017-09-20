#include "LuaUtil.h"

void FLuaUtil::RegisterClass(const luaL_Reg *ClassFunctions, const char *ClassName)
{
	AddClass(ClassName);
	OpenClass(ClassName);
	RegisterClassFunctions(ClassFunctions);
	CloseClass();
}

void FLuaUtil::AddClass(const char *ClassName)
{ // ��class��Ϊtable,��ӵ�luaȫ�ֱ�����
	if (ExistClass(ClassName))
	{
		return;
	}

	// ����Ԫ��moduleName,������ȫ�ֱ�(l_gt)��
	lua_pushvalue(g_LuaState, LUA_GLOBALSINDEX); // ��l_gt��,����ջ��
	lua_pushstring(g_LuaState, ClassName); // ��moduleName��ջ,��ΪKey
	luaL_newmetatable(g_LuaState, ClassName);// ����Ԫ��,��ѹջ

	{// ���ñ�����
		InitMetaFuncs();  // ����Ԫ����
		InitUserDefinedFuncs(ClassName); // �����û��Զ������
	}

	lua_rawset(g_LuaState, -3); // ��ȫ�ֱ�ModuleName����l_gt����,��������������
	lua_pop(g_LuaState, 1); // ��ջ��l_gt����
}

void FLuaUtil::OpenClass(const char *ClassName)
{
	lua_pushvalue(g_LuaState, LUA_GLOBALSINDEX); // ��l_gt��,����ջ��
	lua_pushstring(g_LuaState, ClassName); // ��moduleName��ջ,��ΪKey
	lua_rawget(g_LuaState, -2);
}

void FLuaUtil::CloseClass()
{
	lua_pop(g_LuaState, 2);
}

void FLuaUtil::RegisterClassFunctions( luaL_Reg *ClassFunctions)
{ // ע�ắ��
	for (int32 i=0; ClassFunctions[i].name!=nullptr; ++i)
	{
		if (ClassFunctions[i].func!=nullptr)
		{
			lua_CFunction func = ClassFunctions->func;
			AddClassFunction(ClassFunctions[i].name, func);
		}
	}
}

void FLuaUtil::AddClassFunction(const char *FuncName, lua_CFunction &luaFunction)
{
	lua_pushstring(g_LuaState, FuncName);
	lua_pushcfunction(g_LuaState, luaFunction);
	lua_rawset(g_LuaState, -3);
}

void FLuaUtil::InitMetaFuncs()
{

}

void FLuaUtil::InitUserDefinedFuncs(const char *ClassName)
{
	{ // �趨����,�Ƿ���cppclass
		lua_pushstring(g_LuaState, "IsCppClass");
		lua_pushboolean(g_LuaState, true);
		lua_rawset(g_LuaState, -3);
	}

	{ // ��ȡclassName
		lua_pushstring(g_LuaState, "ClassName");
		lua_pushstring(g_LuaState, ClassName);
		lua_rawset(g_LuaState, -3);
	}

}

bool FLuaUtil::ExistData(void *p)
{
	lua_getfield(g_LuaState, LUA_REGISTRYINDEX, "_existuserdata");
	lua_pushlightuserdata(g_LuaState, p);
	lua_rawget(g_LuaState, -2);
	bool bExist = lua_isnil(g_LuaState, -1);
	lua_pop(g_LuaState, 2);
	return bExist;
}

bool FLuaUtil::ExistClass(const char *ClassName)
{
	lua_getglobal(g_LuaState, ClassName);
	bool bExistClass = true;
	if (lua_istable(g_LuaState), -1)
	{
		bExistClass = false;
		LuaWrapperLog(Warning, TEXT("class:%s not export!!!"), ClassName);
	}
	
	lua_pop(g_LuaState, 1);
	return bExistClass;
}

int32 FLuaUtil::Push(uint8 value)
{
	lua_pushboolean(g_LuaState, !!value);
	return 1;
}

int32 FLuaUtil::Push(int value)
{
	lua_pushinteger(g_LuaState, value);
	return 1;
}

int32 FLuaUtil::Push(float value)
{
	lua_pushnumber(g_LuaState, value);
	return 1;
}

int32 FLuaUtil::Push(double value)
{
	lua_pushnumber(g_LuaState, value);
	return 1;
}

int32 FLuaUtil::Push(bool value)
{
	lua_pushboolean(g_LuaState, value);
	return 1;
}

int32 FLuaUtil::Push(const FString& value)
{
	lua_pushstring(g_LuaState, TCHAR_TO_ANSI(*value));
	return 1;
}

int32 FLuaUtil::Push(const FText& value)
{
	lua_pushstring(g_LuaState, TCHAR_TO_ANSI(*value.ToString()));
	return 1;
}

int32 FLuaUtil::Push(const FName& value)
{
	lua_pushstring(g_LuaState, TCHAR_TO_ANSI(*value.ToString()));
	return 1;
}

int32 FLuaUtil::Push(const char* value)
{
	lua_pushstring(g_LuaState, value);
	return 1;
}

int32 FLuaUtil::Push(const FClassType &&value)
{ // push class args
	if (!ExistClass(value.m_ClassName))
	{
		LuaWrapperLog(Fatal, TEXT("can not use a not export class(%s) as the lua arg"), value.m_ClassName);
		return 1;
	}

	if (value.m_ClassObj==nullptr)
	{
		lua_pushnil(g_LuaState);
		return 1;
	}

	if (!ExistData((void*)value.m_ClassObj))
	{ // add to table
		*(void**)lua_newuserdata(g_LuaState, sizeof(void *)) = value.m_ClassObj;
		lua_getfield(g_LuaState, LUA_REGISTRYINDEX, "_existuserdata");
		lua_pushlightuserdata(g_LuaState, value.m_ClassObj);
		lua_pushvalue(g_LuaState, -3);
		lua_rawset(g_LuaState, -3);
		lua_pop(g_LuaState, 2); // pop the LUA_REGISTRYINDEX table and userdata
	}

	// set metatable
	lua_getfield(g_LuaState, LUA_REGISTRYINDEX, "_existuserdata");
	lua_pushlightuserdata(g_LuaState, (void*)value.m_ClassObj);
	lua_rawget(g_LuaState, -2); // get userdata
	luaL_getmetatable(g_LuaState, value.m_ClassName);
	lua_setmetatable(g_LuaState, -2);
	lua_replace(L, -2);
	return 1;
}

void FLuaUtil::pop()
{
	lua_pop(g_LuaState, 1);
}

void FLuaUtil::pop(uint8 &ReturnValue)
{
	ReturnValue = lua_tointeger(g_LuaState, -1);
	pop();
}

void FLuaUtil::pop(int32 &ReturnValue)
{
	ReturnValue = lua_tointeger(g_LuaState, -1);
	pop();
}

void FLuaUtil::pop(int &ReturnValue)
{
	ReturnValue = lua_tointeger(g_LuaState, -1);
	pop();
}

void FLuaUtil::pop(float &ReturnValue)
{
	ReturnValue = lua_tonumber(g_LuaState, -1);
	pop();
}

void FLuaUtil::pop(double &ReturnValue)
{
	ReturnValue = lua_tonumber(g_LuaState, -1);
	pop();
}

void FLuaUtil::pop(bool &ReturnValue)
{
	ReturnValue = lua_toboolean(g_LuaState, -1);
	pop();
}

void FLuaUtil::pop(FText &ReturnValue)
{
	ReturnValue = FText::FromString(luaL_checkstring(g_LuaState, -1));
	pop();
}

void FLuaUtil::pop(FName &ReturnValue)
{
	ReturnValue = FName(luaL_checkstring(g_LuaState, -1));
	pop();
}

void FLuaUtil::pop(FString &ReturnValue)
{
	ReturnValue = FString(ANSI_TO_TCHAR(luaL_checkstring(g_LuaState, -1)));
	pop();
}

int LuaErrHandleFunc(lua_State*LuaState)
{
	lua_getfield(LuaState, LUA_GLOBALSINDEX, "debug");
	lua_getfield(LuaState, -1, "traceback");
	lua_pushthread(LuaState);
	lua_pushvalue(LuaState, 1);
	lua_pushinteger(LuaState, 2);
	lua_call(LuaState, 3, 1);
	lua_remove(LuaState, -2);
	lua_concat(LuaState, 2);
	lua_getfield(LuaState, LUA_GLOBALSINDEX, "ErrHandleInLua");
	if (lua_isnil(LuaState, -1))
	{
		lua_pop(LuaState, 1);
	}
	else
	{
		lua_pushvalue(LuaState, -2);
		lua_call(LuaState, 1, 0);
	}
	return 1;
}
