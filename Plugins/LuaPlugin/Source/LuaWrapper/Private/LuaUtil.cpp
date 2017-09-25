#include "LuaUtil.h"

void FLuaUtil::RegisterClass(const luaL_Reg ClassFunctions[], const char *ClassName)
{
	AddClass(ClassName);
	OpenClass(ClassName);
	RegisterClassFunctions(ClassFunctions);
	CloseClass();
}

void FLuaUtil::AddClass(const char *ClassName)
{ // 将class作为table,添加到lua全局变量中
	if (ExistClass(ClassName))
	{
		return;
	}

	// 创建元表moduleName,并放入全局表(l_gt)中
	lua_pushvalue(g_LuaState, LUA_GLOBALSINDEX); // 打开l_gt表,放入栈顶
	lua_pushstring(g_LuaState, ClassName); // 把moduleName入栈,作为Key
	luaL_newmetatable(g_LuaState, ClassName);// 创建元表,并压栈

	{// 设置表内容
		InitMetaFuncs();  // 设置元操作
		InitUserDefinedFuncs(ClassName); // 设置用户自定义操作
	}

	lua_rawset(g_LuaState, -3); // 将全局表ModuleName放入l_gt表中,并弹出两个参数
	lua_pop(g_LuaState, 1); // 把栈顶l_gt表弹出
}

void FLuaUtil::OpenClass(const char *ClassName)
{
	lua_pushvalue(g_LuaState, LUA_GLOBALSINDEX); // 打开l_gt表,放入栈顶
	lua_pushstring(g_LuaState, ClassName); // 把moduleName入栈,作为Key
	lua_rawget(g_LuaState, -2);
}

void FLuaUtil::CloseClass()
{
	lua_pop(g_LuaState, 2);
}

void FLuaUtil::RegisterClassFunctions( const luaL_Reg ClassFunctions[])
{ // 注册函数
	for (int32 i=0; ClassFunctions[i].name!=nullptr; ++i)
	{
		if (ClassFunctions[i].func!=nullptr)
		{
			lua_CFunction func = ClassFunctions[i].func;
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
	{ // 设定变量,是否是cppclass
		lua_pushstring(g_LuaState, "IsCppClass");
		lua_pushboolean(g_LuaState, true);
		lua_rawset(g_LuaState, -3);
	}

	{ // 获取className
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
	bool bExistClass = lua_istable(g_LuaState, -1)==1;
	lua_pop(g_LuaState, 1);
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

int32 FLuaUtil::Push(uint8 value)
{
	lua_pushboolean(g_LuaState, !!value);
	return 1;
}

int32 FLuaUtil::Push(int32 value)
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

int32 FLuaUtil::Push(const char* value)
{
	lua_pushstring(g_LuaState, value);
	return 1;
}


int32 FLuaUtil::Push()
{
	return 0;
}

int32 FLuaUtil::Push(const FLuaClassType<uint8> &&value)
{
	lua_pushinteger(g_LuaState, value.m_ClassObj);
	return 1;
}

int32 FLuaUtil::Push(const FLuaClassType<int32> &&value)
{
	lua_pushinteger(g_LuaState, value.m_ClassObj);
	return 1;
}

int32 FLuaUtil::Push(const FLuaClassType<float> &&value)
{
	lua_pushnumber(g_LuaState, value.m_ClassObj);
	return 1;
}

int32 FLuaUtil::Push(const FLuaClassType<double> &&value)
{
	lua_pushnumber(g_LuaState, value.m_ClassObj);
	return 1;
}

int32 FLuaUtil::Push(const FLuaClassType<bool> &&value)
{
	lua_pushboolean(g_LuaState, value.m_ClassObj);
	return 1;
}

int32 FLuaUtil::Push(const FLuaClassType<const char*> &&value)
{
	lua_pushstring(g_LuaState, value.m_ClassObj);
	return 1;
}

int32 FLuaUtil::Push(const FLuaClassType<const FString&> &&value)
{
	lua_pushstring(g_LuaState, TCHAR_TO_ANSI(*value.m_ClassObj));
	return 1;
}

void FLuaUtil::Pop()
{
	lua_pop(g_LuaState, 1);
}

void FLuaUtil::Pop(uint8 &ReturnValue)
{
	ReturnValue = lua_tointeger(g_LuaState, -1);
	Pop();
}

void FLuaUtil::Pop(float &ReturnValue)
{
	ReturnValue = lua_tonumber(g_LuaState, -1);
	Pop();
}

void FLuaUtil::Pop(double &ReturnValue)
{
	ReturnValue = lua_tonumber(g_LuaState, -1);
	Pop();
}

void FLuaUtil::Pop(bool &ReturnValue)
{
	ReturnValue = !!(lua_toboolean(g_LuaState, -1));
	Pop();
}

void FLuaUtil::Pop(FText &ReturnValue)
{
	ReturnValue = FText::FromString(luaL_checkstring(g_LuaState, -1));
	Pop();
}

void FLuaUtil::Pop(FName &ReturnValue)
{
	ReturnValue = FName(luaL_checkstring(g_LuaState, -1));
	Pop();
}

void FLuaUtil::Pop(FString &ReturnValue)
{
	ReturnValue = FString(ANSI_TO_TCHAR(luaL_checkstring(g_LuaState, -1)));
	Pop();
}

void FLuaUtil::Pop(int32 &ReturnValue)
{
	ReturnValue = lua_tointeger(g_LuaState, -1);
	Pop();
}

void FLuaUtil::Pop(FLuaClassType<uint8> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_tointeger(g_LuaState, -1);
	Pop();
}

void FLuaUtil::Pop(FLuaClassType<int32> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_tointeger(g_LuaState, -1);
	Pop();
}

void FLuaUtil::Pop(FLuaClassType<float> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_tonumber(g_LuaState, -1);
	Pop();
}

void FLuaUtil::Pop(FLuaClassType<double> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_tonumber(g_LuaState, -1);
	Pop();
}

void FLuaUtil::Pop(FLuaClassType<bool> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_toboolean(g_LuaState, -1)==1;
	Pop();
}

void FLuaUtil::Pop(FLuaClassType<FText> &&ReturnValue)
{
	ReturnValue.m_ClassObj = FText::FromString(ANSI_TO_TCHAR(lua_tostring(g_LuaState, -1)));
	Pop();
}

void FLuaUtil::Pop(FLuaClassType<FName> &&ReturnValue)
{
	ReturnValue.m_ClassObj = FName(ANSI_TO_TCHAR(lua_tostring(g_LuaState, -1)));
	Pop();
}

void FLuaUtil::Pop(FLuaClassType<FString> &&ReturnValue)
{
	ReturnValue.m_ClassObj = FString(ANSI_TO_TCHAR(lua_tostring(g_LuaState, -1)));
	Pop();
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
