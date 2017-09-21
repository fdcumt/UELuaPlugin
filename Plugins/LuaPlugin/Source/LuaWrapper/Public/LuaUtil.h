#pragma once
#include "lua_tinker.h"
#include "LuaWrapperDefine.h"

LUAWRAPPER_API int LuaErrHandleFunc(lua_State*g_LuaState);

class LUAWRAPPER_API FLuaFuncName
{
public:
	explicit FLuaFuncName(const char *FuncName)
		:m_FuncName(FuncName)
	{
	}

public:
	const char *m_FuncName;
};

class FLuaReturnTypeNum
{
public:
	explicit FLuaReturnTypeNum(int32 num)
		:m_num(num)
	{
	}

	FLuaReturnTypeNum& operator +(int32 num )
	{
		m_num += num;
		return *this;
	}

public:
	int32 m_num;
};

template <class T>
class LUAWRAPPER_API FLuaClassType
{
public:
	explicit FLuaClassType(T &ClassObj, const char *ClassName)
		:m_ClassObj(ClassObj)
		,m_ClassName(ClassName)
	{
	}

public:
	T &m_ClassObj;
	const char *m_ClassName;
};

class LUAWRAPPER_API FLuaUtil
{
public:
	static void RegisterClass(const luaL_Reg ClassFunctions[], const char *ClassName);


public: // call functions 
	template <class... T>
	static void Call(const FString &FuncName, T&&... args)
	{
		Call(TCHAR_TO_ANSI(*FuncName), Forward<T>(args)...);
	}

	template <class... T>
	static void Call(const char *FuncName, T&&... args)
	{
		CallRImpl(FLuaReturnTypeNum(0), FLuaFuncName(FuncName), Forward<T>(args)...);
// 		lua_pushcfunction(g_LuaState, LuaErrHandleFunc);
// 		lua_getfield(g_LuaState, LUA_GLOBALSINDEX, FuncName);
// 		int32 paramCount = Push(Forward<T>(args)...);
// 		if (lua_pcall(g_LuaState, paramCount, 0, -(paramCount+2)))
// 		{
// 			FString log = FString::Printf(TEXT("call r impl found an error: %s"), ANSI_TO_TCHAR(lua_tostring(g_LuaState, -1)));
// 			TemplateLogFatal(log);
// 		}
	}


public: // call function with return
	template <class... T>
	static void CallR(T&&... args)
	{
		CallRImpl(FLuaReturnTypeNum(0), Forward<T>(args)...);
	}

private:
	template <class... T>
	static void CallRImpl(FLuaReturnTypeNum &&RetTypeNum, FLuaFuncName &&Value, T&&... args)
	{
		lua_pushcfunction(g_LuaState, LuaErrHandleFunc);
		lua_getfield(g_LuaState, LUA_GLOBALSINDEX, Value.m_FuncName);
		int32 paramCount = Push(Forward<T>(args)...);
		if (lua_pcall(g_LuaState, paramCount, RetTypeNum.m_num, -(paramCount + 2)))
		{
			FString log = FString::Printf(TEXT("call r impl found an error: %s!!!"), ANSI_TO_TCHAR(lua_tostring(g_LuaState, -1)));
			TemplateLogFatal(log);
		}
	}

	template <class T1, class... T>
	static void CallRImpl(FLuaReturnTypeNum &&RetTypeNum, T1 &&ReturnValue, T&&... args)
	{
		CallRImpl(Forward<FLuaReturnTypeNum>(RetTypeNum+1), Forward<T>(args)...);
		Pop(ReturnValue);
	}


public:
	template <class T>
	static void TouserCppClassType(FLuaClassType<T> &&OutValue); // 解析成 cpp class 类型


public: // push args
	template <class T1, class... T>
	static int32 Push(const T1 &Value, T&&... args);

	template<typename T>
	static int Push(const T& value);

	static int32 Push();
	static int32 Push(uint8  value);
	static int32 Push(int    value);
	static int32 Push(float  value);
	static int32 Push(double value);
	static int32 Push(bool   value);
	static int32 Push(const char*    value);
	static int32 Push(const FText&   value);
	static int32 Push(const FName&   value);
	static int32 Push(const FString& value);

	template <class T>
	static int32 Push(const FLuaClassType<T> &&value);


public:
	static void Pop();
	static void Pop(uint8  &ReturnValue);
	static void Pop(float  &ReturnValue);
	static void Pop(double &ReturnValue);
	static void Pop(bool   &ReturnValue);
	static void Pop(FText   &ReturnValue);
	static void Pop(FName   &ReturnValue);
	static void Pop(FString &ReturnValue);
	static void Pop(int32   &ReturnValue);

	template <class T>
	static void Pop(FLuaClassType<T> &&ReturnValue);


private: // not export Function
	static void AddClass(const char *ClassName);
	static void OpenClass(const char *ClassName);
	static void CloseClass();
	static void RegisterClassFunctions( const luaL_Reg ClassFunctions[]);
	static void AddClassFunction(const char *FuncName, lua_CFunction &luaFunction);
	static void InitMetaFuncs(); // 初始化元表中的元操作
	static void InitUserDefinedFuncs(const char *ClassName); // 初始化元表中的自定义操作
	static bool ExistData(void *p);
	static bool ExistClass(const char *ClassName);

private: // log
	static void TemplateLogPrint(const FString &Content);
	static void TemplateLogWarning(const FString &Content);
	static void TemplateLogError(const FString &Content);
	static void TemplateLogFatal(const FString &Content);

};


template <class T1, class... T>
int32 FLuaUtil::Push(const T1 &Value ,T&&... args)
{
	int32 num = Push(Value);
	return num + Push(Forward<T>(args)...);
}

template<typename T>
int FLuaUtil::Push(const T& value)
{
	TemplateLogFatal(TEXT("not find this arg type"));
}

template <class T>
void FLuaUtil::Pop(FLuaClassType<T> &&ReturnValue)
{
	const char *ClassName = ReturnValue.m_ClassName;
	if (!ExistClass(ReturnValue.m_ClassName))
	{
		TemplateLogFatal(FString::Printf(TEXT("can not Pop class %s, it not export!!!"), ReturnValue.m_ClassName));
		return;
	}

	TouserCppClassType(ReturnValue);
}

template <class T>
void FLuaUtil::TouserCppClassType(FLuaClassType<T> &&OutValue)
{
	if (lua_isnil(g_LuaState, -1))
	{
		OutValue.m_ClassObj = nullptr;
	}
	else if (lua_istable(g_LuaState, -1))
	{
		lua_pushstring(g_LuaState, "CppParent");
		lua_rawget(g_LuaState, -2);
		lua_replace(g_LuaState, -2);
		TouserCppClassType(OutValue);
	}
	else if (lua_isuserdata(g_LuaState, -1))
	{
		OutValue.m_ClassObj = static_cast<T>(lua_touserdata(L, -1))
	}
	else
	{
		TemplateLogFatal(TEXT("TouserCppClassType error"));
	}
}

template <class T>
int32 FLuaUtil::Push(const FLuaClassType<T> &&value)
{ // push class args
	if (!ExistClass(value.m_ClassName))
	{
		FString log = FString::Printf(TEXT("push error, not export this class:%s"), value.m_ClassName);
		TemplateLogFatal(log);
		return 1;
	}

	if (value.m_ClassObj == nullptr)
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
		lua_pop(g_LuaState, 2); // Pop the LUA_REGISTRYINDEX table and userdata
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

