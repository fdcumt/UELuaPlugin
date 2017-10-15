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
			TemplateLogError(log);
		}
	}

	template <class T1, class... T>
	static void CallRImpl(FLuaReturnTypeNum &&RetTypeNum, T1 &&ReturnValue, T&&... args)
	{
		CallRImpl(Forward<FLuaReturnTypeNum>(RetTypeNum+1), Forward<T>(args)...);
		TouserData(g_LuaState, -1, ReturnValue);
		lua_pop(g_LuaState, 1);
	}

public:
	template <class T>
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<T> &&OutValue);

	template <class T>
	static T TouserData(lua_State *InLuaState, const int32 LuaStackIndex, const char *ClassName)
	{
		T pCppObj;
		TouserData(InLuaState, LuaStackIndex, FLuaClassType<T>(pCppObj, ClassName));
		return pCppObj;
	}

	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, uint8   &ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, float   &ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, double  &ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, bool    &ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FText   &ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FName   &ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FString &ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, int32   &ReturnValue);

	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<uint8>   &&ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<int32>   &&ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<float>   &&ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<double>  &&ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<bool>    &&ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<FText>   &&ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<FName>   &&ReturnValue);
	static void TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<FString> &&ReturnValue);

private:
	template <class T>
	static void TouserDataInner(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<T> &&OutValue);

public: // push args
	template <class T1, class... T>
	static int32 Push(lua_State *InLuaState, const T1 &Value, T&&... args)
	{
		int32 num = Push(Value);
		return num + Push(Forward<T>(args)...);
	}

	template<typename T>
	static int Push(lua_State *InLuaState, const T& value)
	{
		TemplateLogError(TEXT("not find this arg type"));
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

private: // not export Function
	static void AddClass(lua_State *InLuaState, const char *ClassName);
	static void OpenClass(lua_State *InLuaState, const char *ClassName);
	static void CloseClass(lua_State *InLuaState);
	static void RegisterClassFunctions( lua_State *InLuaState, const luaL_Reg ClassFunctions[]);
	static void AddClassFunction(lua_State *InLuaState, const char *FuncName, lua_CFunction &luaFunction);
	static void InitMetaMethods(lua_State *InLuaState); // 初始化元表中的元操作
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
		TemplateLogError(log);
		return 1;
	}

	if (value.m_ClassObj == nullptr)
	{
		lua_pushnil(InLuaState);
		return 1;
	}

	if (!ExistData(InLuaState, (void*)value.m_ClassObj))
	{ // add to table
	  // 这个函数分配分配一块指定大小的内存块， 把内存块地址作为一个完整的 userdata 压入堆栈，并返回这个地址。
		*(T*)lua_newuserdata(InLuaState, sizeof(T)) = value.m_ClassObj;
		// 把 t[k] 值压入堆栈
		lua_getfield(InLuaState, LUA_REGISTRYINDEX, "_existuserdata");
		lua_pushlightuserdata(InLuaState, value.m_ClassObj);
		lua_pushvalue(InLuaState, -3);
		lua_rawset(InLuaState, -3);
		lua_pop(InLuaState, 2); // Pop the LUA_REGISTRYINDEX table and userdata
	}

	// set metatable
	lua_getfield(InLuaState, LUA_REGISTRYINDEX, "_existuserdata");
	lua_pushlightuserdata(InLuaState, value.m_ClassObj);
	lua_rawget(InLuaState, -2); // get userdata
	luaL_getmetatable(InLuaState, value.m_ClassName);
	lua_setmetatable(InLuaState, -2);
	lua_replace(InLuaState, -2);

	return 1;
}

template <class T>
void FLuaUtil::TouserData(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<T> &&ReturnValue)
{
	const char *ClassName = ReturnValue.m_ClassName;
	if (!ExistClass(InLuaState, LuaStackIndex, ReturnValue.m_ClassName))
	{
		TemplateLogError(FString::Printf(TEXT("can not Pop class %s, it not export!!!"), ReturnValue.m_ClassName));
		return;
	}

	TouserDataInner(InLuaState, LuaStackIndex, Forward<FLuaClassType<T>>(ReturnValue));
}


// get value from lua stack where the position is luaStackIndex
template <class T>
void FLuaUtil::TouserDataInner(lua_State *InLuaState, const int32 LuaStackIndex, FLuaClassType<T> &&OutValue)
{
	if (lua_isnil(InLuaState, LuaStackIndex))
	{
		OutValue.m_ClassObj = nullptr;
	}
	else if (lua_isuserdata(InLuaState, LuaStackIndex) == 1)
	{
		OutValue.m_ClassObj = *(static_cast<T*>(lua_touserdata(InLuaState, LuaStackIndex)));
	}
	else if (lua_istable(InLuaState, LuaStackIndex))
	{
		lua_pushstring(InLuaState, "CppParent");
		lua_rawget(InLuaState, LuaStackIndex);
		TouserDataInner(InLuaState, -1, Forward<FLuaClassType<T>>(OutValue) );
		lua_pop(InLuaState, 1);
	}
	else
	{
		TemplateLogError(TEXT("TouserData error"));
	}
}
