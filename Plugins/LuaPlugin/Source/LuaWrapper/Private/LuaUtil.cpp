#include "LuaUtil.h"

void FLuaUtil::RegisterClass(lua_State *InLuaState, const luaL_Reg ClassFunctions[], const char *ClassName)
{
	AddClass(InLuaState, ClassName);
	OpenClass(InLuaState,ClassName);
	RegisterClassFunctions(InLuaState,ClassFunctions);
	CloseClass(InLuaState);
}

void FLuaUtil::AddClass(lua_State *InLuaState, const char *ClassName)
{ // 将class作为table,添加到lua全局变量中
	if (ExistClass(InLuaState, -1, ClassName))
	{
		return;
	}

	// 创建元表moduleName,并放入全局表(l_gt)中
	lua_pushvalue(InLuaState, LUA_GLOBALSINDEX); // 打开l_gt表,放入栈顶
	lua_pushstring(InLuaState, ClassName); // 把moduleName入栈,作为Key
	luaL_newmetatable(InLuaState, ClassName);// 创建元表,并压栈

	{// 设置表内容
		InitMetaMethods(InLuaState);  // 设置元操作
		InitUserDefinedFuncs(InLuaState, ClassName); // 设置用户自定义操作
	}

	lua_rawset(InLuaState, -3); // 将全局表ModuleName放入l_gt表中,并弹出两个参数
	lua_pop(InLuaState, 1); // 把栈顶l_gt表弹出
}

void FLuaUtil::OpenClass(lua_State *InLuaState, const char *ClassName)
{
	lua_pushvalue(InLuaState, LUA_GLOBALSINDEX); // 打开l_gt表,放入栈顶
	lua_pushstring(InLuaState, ClassName); // 把moduleName入栈,作为Key
	lua_rawget(InLuaState, -2);
}

void FLuaUtil::CloseClass(lua_State *InLuaState)
{
	lua_pop(InLuaState, 2);
}

void FLuaUtil::RegisterClassFunctions( lua_State *InLuaState, const luaL_Reg ClassFunctions[])
{ // 注册函数
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

int32 MetaTableNewIndexFunc(lua_State* L)
{
	// userdata[key] = value;
	// stack 1: userdata
	// stack 2: key
	// stack 3: value
	if (lua_isuserdata(L, 1))
	{
		lua_getmetatable(L, 1);
		FString PropertyName = FString(lua_tostring(L, 2));
		FString SetPropertyFuncName = FString::Printf(TEXT("Set_%s"), *PropertyName);
		FLuaUtil::Push(L, SetPropertyFuncName);
		lua_rawget(L, -2); // get function
		if (!lua_isnil(L, -1))
		{
			lua_pushvalue(L, 1);
			lua_pushvalue(L, 3);
			lua_call(L, 2, 0);
		}
	}
	else if (lua_istable(L, 1))
	{
		lua_rawset(L, -3);
	}
	return 0;
}


int32 MetaTableIndexFunc(lua_State* L)
{
	// userdata[key];
	// stack 1: userdata
	// stack 2: key
	lua_getmetatable(L, 1);
	lua_pushvalue(L, -2);
	lua_rawget(L, -2);
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		FString PropertyName = FString(lua_tostring(L, 2));
		FString GetPropertyFuncName = FString::Printf(TEXT("Get_%s"), *PropertyName);
		FLuaUtil::Push(L, GetPropertyFuncName);
		lua_rawget(L, -2); // get function
		if (!lua_isnil(L, -1))
		{
			lua_pushvalue(L, 1);
			lua_call(L, 1, 1);
		}
	}
	return 1;
}

void FLuaUtil::InitMetaMethods(lua_State *InLuaState)
{
// 	lua_pushvalue(InLuaState, -1);
// 	lua_setfield(InLuaState, -2, "__index");
	lua_pushstring(InLuaState, "__index");
	lua_pushcfunction(InLuaState, MetaTableIndexFunc);
	lua_rawset(InLuaState, -3);
	lua_pushstring(InLuaState, "__newindex");
	lua_pushcfunction(InLuaState, MetaTableNewIndexFunc);
	lua_rawset(InLuaState, -3);

}

void FLuaUtil::InitUserDefinedFuncs(lua_State *InLuaState, const char *ClassName)
{
	{ // 设定变量,是否是cppclass
		lua_pushstring(InLuaState, "IsCppClass");
		lua_pushboolean(InLuaState, true);
		lua_rawset(InLuaState, -3);
	}

	{ // 获取className
		lua_pushstring(InLuaState, "ClassName");
		lua_pushstring(InLuaState, ClassName);
		lua_rawset(InLuaState, -3);
	}

}

bool FLuaUtil::ExistData(lua_State *InLuaState, const FString &UserDataKey)
{
	lua_getfield(InLuaState, LUA_REGISTRYINDEX, "_existuserdata");
	Push(InLuaState, UserDataKey);
	lua_rawget(InLuaState, -2);
	bool bExist = lua_isnil(InLuaState, -1)==0;
	lua_pop(InLuaState, 2);
	return bExist;
}

bool FLuaUtil::ExistClass(lua_State *InLuaState, const int32 LuaStackIndex, const char *ClassName)
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

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, uint8 &ReturnValue)
{
	ReturnValue = lua_tointeger(InLuaState, LuaStackIndex);
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, float &ReturnValue)
{
	ReturnValue = lua_tonumber(InLuaState, LuaStackIndex);
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, double &ReturnValue)
{
	ReturnValue = lua_tonumber(InLuaState, LuaStackIndex);
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, bool &ReturnValue)
{
	ReturnValue = !!(lua_toboolean(InLuaState, LuaStackIndex));
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FText &ReturnValue)
{
	ReturnValue = FText::FromString(luaL_checkstring(InLuaState, LuaStackIndex));
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FName &ReturnValue)
{
	ReturnValue = FName(luaL_checkstring(InLuaState, LuaStackIndex));
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FString &ReturnValue)
{
	ReturnValue = FString(ANSI_TO_TCHAR(luaL_checkstring(InLuaState, LuaStackIndex)));
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, int32 &ReturnValue)
{
	ReturnValue = lua_tointeger(InLuaState, LuaStackIndex);
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<uint8> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_tointeger(InLuaState, LuaStackIndex);
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<int32> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_tointeger(InLuaState, LuaStackIndex);
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<float> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_tonumber(InLuaState, LuaStackIndex);
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<double> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_tonumber(InLuaState, LuaStackIndex);
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<bool> &&ReturnValue)
{
	ReturnValue.m_ClassObj = lua_toboolean(InLuaState, LuaStackIndex)==1;
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<FText> &&ReturnValue)
{
	ReturnValue.m_ClassObj = FText::FromString(ANSI_TO_TCHAR(lua_tostring(InLuaState, LuaStackIndex)));
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<FName> &&ReturnValue)
{
	ReturnValue.m_ClassObj = FName(ANSI_TO_TCHAR(lua_tostring(InLuaState, LuaStackIndex)));
}

void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<FString> &&ReturnValue)
{
	ReturnValue.m_ClassObj = FString(ANSI_TO_TCHAR(lua_tostring(InLuaState, LuaStackIndex)));
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
