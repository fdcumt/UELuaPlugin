#include "LuaUtil.h"

void FLuaUtil::RegisterClass(lua_State *InLuaState, const luaL_Reg ClassFunctions[], const char *ClassName)
{
	AddClass(InLuaState, ClassName);
	OpenClass(InLuaState,ClassName);
	RegisterClassFunctions(InLuaState,ClassFunctions);
	CloseClass(InLuaState);
}

void FLuaUtil::AddClass(lua_State *InLuaState, const char *ClassName)
{ // ��class��Ϊtable,��ӵ�luaȫ�ֱ�����
	if (ExistClass(InLuaState, ClassName))
	{
		return;
	}

	// ����Ԫ��moduleName,������ȫ�ֱ�(l_gt)��
	lua_pushvalue(InLuaState, LUA_GLOBALSINDEX); // ��l_gt��,����ջ��
	lua_pushstring(InLuaState, ClassName); // ��moduleName��ջ,��ΪKey
	luaL_newmetatable(InLuaState, ClassName);// ����Ԫ��,��ѹջ

	{// ���ñ�����
		InitMetaFuncs(InLuaState);  // ����Ԫ����
		InitUserDefinedFuncs(InLuaState, ClassName); // �����û��Զ������
	}

	lua_rawset(InLuaState, -3); // ��ȫ�ֱ�ModuleName����l_gt����,��������������
	lua_pop(InLuaState, 1); // ��ջ��l_gt����
}

void FLuaUtil::OpenClass(lua_State *InLuaState, const char *ClassName)
{
	lua_pushvalue(InLuaState, LUA_GLOBALSINDEX); // ��l_gt��,����ջ��
	lua_pushstring(InLuaState, ClassName); // ��moduleName��ջ,��ΪKey
	lua_rawget(InLuaState, -2);
}

void FLuaUtil::CloseClass(lua_State *InLuaState)
{
	lua_pop(InLuaState, 2);
}

void FLuaUtil::RegisterClassFunctions( lua_State *InLuaState, const luaL_Reg ClassFunctions[])
{ // ע�ắ��
	for (int32 i=0; ClassFunctions[i].name!=nullptr; ++i)
	{
		if (ClassFunctions[i].func!=nullptr)
		{
			lua_CFunction func = ClassFunctions[i].func;
			AddClassFunction(InLuaState, ClassFunctions[i].name, func);
		}
	}
}

void FLuaUtil::AddClassFunction(lua_State *InLuaState, const char *FuncName, lua_CFunction &luaFunction)
{
	lua_pushstring(InLuaState, FuncName);
	lua_pushcfunction(InLuaState, luaFunction);
	lua_rawset(InLuaState, -3);
}

void FLuaUtil::InitMetaFuncs(lua_State *InLuaState)
{
	lua_pushvalue(InLuaState, -1);
	lua_setfield(InLuaState, -2, "__index");
}

void FLuaUtil::InitUserDefinedFuncs(lua_State *InLuaState, const char *ClassName)
{
	{ // �趨����,�Ƿ���cppclass
		lua_pushstring(InLuaState, "IsCppClass");
		lua_pushboolean(InLuaState, true);
		lua_rawset(InLuaState, -3);
	}

	{ // ��ȡclassName
		lua_pushstring(InLuaState, "ClassName");
		lua_pushstring(InLuaState, ClassName);
		lua_rawset(InLuaState, -3);
	}

}

bool FLuaUtil::ExistData(lua_State *InLuaState, void *p)
{
	lua_getfield(InLuaState, LUA_REGISTRYINDEX, "_existuserdata");
	lua_pushlightuserdata(InLuaState, p);
	lua_rawget(InLuaState, -2);
	bool bExist = !lua_isnil(InLuaState, -1);
	lua_pop(InLuaState, 2);
	return bExist;
}

bool FLuaUtil::ExistClass(lua_State *InLuaState, const char *ClassName)
{
	lua_getglobal(InLuaState, ClassName);
	bool bExistClass = lua_istable(InLuaState, -1)==1;
	lua_pop(InLuaState, 1);
	return bExistClass;
}

void FLuaUtil::TemplateLogPrint(const FString &Content)
{
	LuaWrapperLog(Log, TEXT("%s"), *Content);
}

void FLuaUtil::TemplateLogWarning(const FString &Content)
{
	LuaWrapperLog(Warning, TEXT("%s"), *Content);
}

void FLuaUtil::TemplateLogError(const FString &Content)
{
	LuaWrapperLog(Error, TEXT("%s"), *Content);
}

void FLuaUtil::TemplateLogFatal(const FString &Content)
{
	LuaWrapperLog(Fatal, TEXT("%s"), *Content);
}

int32 FLuaUtil::Push(lua_State *InLuaState,uint8 value)
{
	lua_pushboolean(InLuaState, !!value);
	return 1;
}

int32 FLuaUtil::Push(lua_State *InLuaState,int32 value)
{
	lua_pushinteger(InLuaState, value);
	return 1;
}

int32 FLuaUtil::Push(lua_State *InLuaState, float value)
{
	lua_pushnumber(InLuaState, value);
	return 1;
}

int32 FLuaUtil::Push(lua_State *InLuaState,double value)
{
	lua_pushnumber(InLuaState, value);
	return 1;
}

int32 FLuaUtil::Push(lua_State *InLuaState,bool value)
{
	lua_pushboolean(InLuaState, value);
	return 1;
}

int32 FLuaUtil::Push(lua_State *InLuaState,const FString& value)
{
	lua_pushstring(InLuaState, TCHAR_TO_ANSI(*value));
	return 1;
}

int32 FLuaUtil::Push(lua_State *InLuaState,const char* value)
{
	lua_pushstring(InLuaState, value);
	return 1;
}


int32 FLuaUtil::Push(lua_State *InLuaState)
{
	return 0;
}

int32 FLuaUtil::Push(lua_State *InLuaState, const FLuaClassType<uint8> &&value)
{
	lua_pushinteger(InLuaState, value.m_ClassObj);
	return 1;
}

int32 FLuaUtil::Push(lua_State *InLuaState, const FLuaClassType<int32> &&value)
{
	lua_pushinteger(InLuaState, value.m_ClassObj);
	return 1;
}

int32 FLuaUtil::Push(lua_State *InLuaState, const FLuaClassType<float> &&value)
{
	lua_pushnumber(InLuaState, value.m_ClassObj);
	return 1;
}

int32 FLuaUtil::Push(lua_State *InLuaState,const FLuaClassType<double> &&value)
{
	lua_pushnumber(InLuaState, value.m_ClassObj);
	return 1;
}

int32 FLuaUtil::Push(lua_State *InLuaState,const FLuaClassType<bool> &&value)
{
	lua_pushboolean(InLuaState, value.m_ClassObj);
	return 1;
}

int32 FLuaUtil::Push(lua_State *InLuaState,const FLuaClassType<const char*> &&value)
{
	lua_pushstring(InLuaState, value.m_ClassObj);
	return 1;
}

int32 FLuaUtil::Push(lua_State *InLuaState,const FLuaClassType<const FString&> &&value)
{
	lua_pushstring(InLuaState, TCHAR_TO_ANSI(*value.m_ClassObj));
	return 1;
}

void FLuaUtil::Pop(lua_State *InLuaState)
{
	lua_pop(InLuaState, 1);
}

void FLuaUtil::Pop(lua_State *InLuaState, uint8 &ReturnValue)
{
	ReturnValue = lua_tointeger(InLuaState, -1);
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, float &ReturnValue)
{
	ReturnValue = lua_tonumber(InLuaState, -1);
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, double &ReturnValue)
{
	ReturnValue = lua_tonumber(InLuaState, -1);
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, bool &ReturnValue)
{
	ReturnValue = !!(lua_toboolean(InLuaState, -1));
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, FText &ReturnValue)
{
	ReturnValue = FText::FromString(luaL_checkstring(InLuaState, -1));
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, FName &ReturnValue)
{
	ReturnValue = FName(luaL_checkstring(InLuaState, -1));
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, FString &ReturnValue)
{
	ReturnValue = FString(ANSI_TO_TCHAR(luaL_checkstring(InLuaState, -1)));
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, int32 &ReturnValue)
{
	ReturnValue = lua_tointeger(InLuaState, -1);
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, FLuaClassType<uint8> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_tointeger(InLuaState, -1);
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, FLuaClassType<int32> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_tointeger(InLuaState, -1);
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, FLuaClassType<float> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_tonumber(InLuaState, -1);
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, FLuaClassType<double> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_tonumber(InLuaState, -1);
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, FLuaClassType<bool> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_toboolean(InLuaState, -1)==1;
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, FLuaClassType<FText> &&ReturnValue)
{
	ReturnValue.m_ClassObj = FText::FromString(ANSI_TO_TCHAR(lua_tostring(InLuaState, -1)));
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, FLuaClassType<FName> &&ReturnValue)
{
	ReturnValue.m_ClassObj = FName(ANSI_TO_TCHAR(lua_tostring(InLuaState, -1)));
	Pop(InLuaState);
}

void FLuaUtil::Pop(lua_State *InLuaState, FLuaClassType<FString> &&ReturnValue)
{
	ReturnValue.m_ClassObj = FString(ANSI_TO_TCHAR(lua_tostring(InLuaState, -1)));
	Pop(InLuaState);
}

int32 LuaErrHandleFunc(lua_State*LuaState)
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
