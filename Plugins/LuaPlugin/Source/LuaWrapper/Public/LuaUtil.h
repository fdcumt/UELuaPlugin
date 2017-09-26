#pragma once
#include "lua_tinker.h"
#include "LuaWrapperDefine.h"

LUAWRAPPER_API int LuaErrHandleFunc(lua_State*InLuaState);

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
	static void RegisterClass(lua_State *InLuaState, const luaL_Reg ClassFunctions[], const char *ClassName);


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
		int32 paramCount = Push(g_LuaState, Forward<T>(args)...);
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
		Pop(g_LuaState, ReturnValue);
	}


public:
	template <class T>
	static void TouserCppClassType(lua_State *InLuaState, FLuaClassType<T> &&OutValue); // 解析成 cpp class 类型

	template <class T>
	static T TouserCppClassType(lua_State *InLuaState, const char *ClassName)
	{
		T pCppObj;
		TouserCppClassType(InLuaState, FLuaClassType<T>(pCppObj, ClassName));
		return pCppObj;
	}


public: // push args
	template <class T1, class... T>
	static int32 Push(lua_State *InLuaState, const T1 &Value, T&&... args)
	{
		int32 num = Push(Value);+
		return num + Push(Forward<T>(args)...);
	}

	template<typename T>
	static int Push(lua_State *InLuaState, const T& value)
	{
		TemplateLogFatal(TEXT("not find this arg type"));
	}

	static int32 Push(lua_State *InLuaState);
	static int32 Push(lua_State *InLuaState, uint8  value);
	static int32 Push(lua_State *InLuaState, int32  value);
	static int32 Push(lua_State *InLuaState, float  value);
	static int32 Push(lua_State *InLuaState, double value);
	static int32 Push(lua_State *InLuaState, bool   value);
	static int32 Push(lua_State *InLuaState, const char*    value);
	static int32 Push(lua_State *InLuaState, const FString& value);

	static int32 Push(lua_State *InLuaState, const FLuaClassType<bool>  &&value);
	static int32 Push(lua_State *InLuaState, const FLuaClassType<uint8> &&value);
	static int32 Push(lua_State *InLuaState, const FLuaClassType<int32> &&value);
	static int32 Push(lua_State *InLuaState, const FLuaClassType<float> &&value);
	static int32 Push(lua_State *InLuaState, const FLuaClassType<double> &&value);
	static int32 Push(lua_State *InLuaState, const FLuaClassType<const char*> &&value);
	static int32 Push(lua_State *InLuaState, const FLuaClassType<const FString&> &&value);

	template <class T>
	static int32 Push(lua_State *InLuaState, const FLuaClassType<T> &&value);



public:
	static void Pop(lua_State *InLuaState);
	static void Pop(lua_State *InLuaState, uint8   &ReturnValue);
	static void Pop(lua_State *InLuaState, float   &ReturnValue);
	static void Pop(lua_State *InLuaState, double  &ReturnValue);
	static void Pop(lua_State *InLuaState, bool    &ReturnValue);
	static void Pop(lua_State *InLuaState, FText   &ReturnValue);
	static void Pop(lua_State *InLuaState, FName   &ReturnValue);
	static void Pop(lua_State *InLuaState, FString &ReturnValue);
	static void Pop(lua_State *InLuaState, int32   &ReturnValue);

	static void Pop(lua_State *InLuaState, FLuaClassType<uint8>   &&ReturnValue);
	static void Pop(lua_State *InLuaState, FLuaClassType<int32>   &&ReturnValue);
	static void Pop(lua_State *InLuaState, FLuaClassType<float>   &&ReturnValue);
	static void Pop(lua_State *InLuaState, FLuaClassType<double>  &&ReturnValue);
	static void Pop(lua_State *InLuaState, FLuaClassType<bool>    &&ReturnValue);
	static void Pop(lua_State *InLuaState, FLuaClassType<FText>   &&ReturnValue);
	static void Pop(lua_State *InLuaState, FLuaClassType<FName>   &&ReturnValue);
	static void Pop(lua_State *InLuaState, FLuaClassType<FString> &&ReturnValue);

	template <class T>
	static void Pop(lua_State *InLuaState, FLuaClassType<T> &&value);
	


private: // not export Function
	static void AddClass(lua_State *InLuaState, const char *ClassName);
	static void OpenClass(lua_State *InLuaState, const char *ClassName);
	static void CloseClass(lua_State *InLuaState);
	static void RegisterClassFunctions( lua_State *InLuaState, const luaL_Reg ClassFunctions[]);
	static void AddClassFunction(lua_State *InLuaState, const char *FuncName, lua_CFunction &luaFunction);
	static void InitMetaFuncs(lua_State *InLuaState); // 初始化元表中的元操作
	static void InitUserDefinedFuncs(lua_State *InLuaState, const char *ClassName); // 初始化元表中的自定义操作
	static bool ExistData(lua_State *InLuaState, void *p);
	static bool ExistClass(lua_State *InLuaState, const char *ClassName);

private: // log
	static void TemplateLogPrint(const FString &Content);
	static void TemplateLogWarning(const FString &Content);
	static void TemplateLogError(const FString &Content);
	static void TemplateLogFatal(const FString &Content);

};

template <class T>
int32 FLuaUtil::Push(lua_State *InLuaState, const FLuaClassType<T> &&value)
{ // push class args
	if (!ExistClass(InLuaState, value.m_ClassName))
	{
		FString log = FString::Printf(TEXT("push error, not export this class:%s"), ANSI_TO_TCHAR(value.m_ClassName));
		TemplateLogFatal(log);
		return 1;
	}

	if (value.m_ClassObj == nullptr)
	{
		lua_pushnil(InLuaState);
		return 1;
	}

	if (!ExistData(InLuaState, (void*)value.m_ClassObj))
	{ // add to table
		*(void**)lua_newuserdata(InLuaState, sizeof(void *)) = value.m_ClassObj;
		lua_getfield(InLuaState, LUA_REGISTRYINDEX, "_existuserdata");
		lua_pushlightuserdata(InLuaState, value.m_ClassObj);
		lua_pushvalue(InLuaState, -3);
		lua_rawset(InLuaState, -3);
		lua_pop(InLuaState, 2); // Pop the LUA_REGISTRYINDEX table and userdata
	}

	// set metatable
	lua_getfield(InLuaState, LUA_REGISTRYINDEX, "_existuserdata");
	lua_pushlightuserdata(InLuaState, (void*)value.m_ClassObj);
	lua_rawget(InLuaState, -2); // get userdata
	luaL_getmetatable(InLuaState, value.m_ClassName);
	lua_setmetatable(InLuaState, -2);
	lua_replace(InLuaState, -2);

	return 1;
}

template <class T>
void FLuaUtil::Pop(lua_State *InLuaState, FLuaClassType<T> &&ReturnValue)
{
	const char *ClassName = ReturnValue.m_ClassName;
	if (!ExistClass(InLuaState, ,ReturnValue.m_ClassName))
	{
		TemplateLogFatal(FString::Printf(TEXT("can not Pop class %s, it not export!!!"), ReturnValue.m_ClassName));
		return;
	}

	TouserCppClassType(InLuaState, ReturnValue);
}

template <class T>
void FLuaUtil::TouserCppClassType(lua_State *InLuaState,  FLuaClassType<T> &&OutValue)
{
	if (lua_isnil(InLuaState, -1))
	{
		OutValue.m_ClassObj = nullptr;
	}
	else if (lua_isuserdata(InLuaState, -1) == 1)
	{
		OutValue.m_ClassObj = static_cast<T>(lua_touserdata(InLuaState, -1));
	}
	else if (lua_istable(InLuaState, -1))
	{
		lua_pushstring(InLuaState, "CppParent");
		lua_rawget(InLuaState, -2);
		lua_replace(InLuaState, -2);
		TouserCppClassType(InLuaState, Forward<FLuaClassType<T>>(OutValue) );
	}
	else
	{
		TemplateLogFatal(TEXT("TouserCppClassType error"));
	}
}
