#pragma once
#include "lua_tinker.h"
#include "LuaWrapperDefine.h"

class LUAWRAPPER_API FLuaFuncName
{
public:
	FLuaFuncName(const char *FuncName)
		:m_FuncName(FuncName)
	{
	}

public:
	const char *m_FuncName;
};

class FLuaReturnTypeNum
{
public:
	FLuaReturnTypeNum(int32 num)
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
class LUAWRAPPER_API FClassType
{
public:
	FClassType(T &ClassObj, const char *ClassName)
		: m_ClassObj(ClassObj)
		, m_ClassName(ClassName)
	{
	}

public:
	T &m_ClassObj;
	const char *m_ClassName;
};

class LUAWRAPPER_API FLuaUtil
{
public:
	static void RegisterClass(const luaL_Reg *ClassFunctions, const char *ClassName);


public: // call functions 
	template <class... T>
	static void Call(const FString &FuncName, T&&... args);

	template <class... T>
	static void Call(const char *FuncName, T&&... args);

public: // push args
	template <class T1, class... T>
	static int32 Push(const T1 &Value, T&&... args);

	template<typename T>
	static int Push(const T& value);

	static int32 Push(uint8  value);
	static int32 Push(int    value);
	static int32 Push(float  value);
	static int32 Push(double value);
	static int32 Push(bool   value);
	static int32 Push(const char*    value);
	static int32 Push(const FText&   value);
	static int32 Push(const FName&   value);
	static int32 Push(const FString& value);
	static int32 Push(const FClassType &&value);


public:
	void pop(uint8  &ReturnValue);
	void pop(int    &ReturnValue);
	void pop(float  &ReturnValue);
	void pop(double &ReturnValue);
	void pop(bool   &ReturnValue);
	void pop(FText   &ReturnValue);
	void pop(FName   &ReturnValue);
	void pop(FString &ReturnValue);
	void pop(int32   &ReturnValue);
	template <class T>
	void pop(FClassType<T> &&ReturnValue);


public: // call function with return
	template <class T1, class... T>
	void CallR(T1 &Value, T&&... args);

private:
	template <class T1, class... T>
	void CallRImpl(FLuaReturnTypeNum &&RetTypeNum,  T1 &&ReturnValue, T&&... args);

	template <class... T>
	void CallRImpl(FLuaReturnTypeNum &&RetTypeNum, const FLuaFuncName &Value, T&&... args);


public:
	template <class T>
	void TouserCppClassType(FClassType<T> &&OutValue); // 解析成 cpp class 类型


private: // not export Function
	static void AddClass(const char *ClassName);
	static void OpenClass(const char *ClassName);
	static void CloseClass();
	static void RegisterClassFunctions( luaL_Reg *ClassFunctions);
	static void AddClassFunction(const char *FuncName, lua_CFunction &luaFunction);
	static void InitMetaFuncs(); // 初始化元表中的元操作
	static void InitUserDefinedFuncs(const char *ClassName); // 初始化元表中的自定义操作
	static bool ExistData(void *p);
	static bool ExistClass(const char *ClassName);

};

int LuaErrHandleFunc(lua_State*g_LuaState);

template <class... T>
void FLuaUtil::Call(const FString &FuncName, T&&... args)
{
	Call(TCHAR_TO_ANSI(*FuncName), Forward<T>(args)...);
}

template <class... T>
void FLuaUtil::Call(const char *FuncName, T&&... args)
{
	CallRImpl(FLuaReturnTypeNum(0), FLuaFuncName(FuncName), Forward<T>(args)...);
}

template <class T1, class... T>
int32 FLuaUtil::Push(const T1 &Value ,T&&... args)
{
	int32 num = Push(Value);
	return num + Push(Forward<T>(args)...);
}

template<typename T>
int FLuaUtil::Push(const T& value)
{
	LuaWrapperLog(Fatal, TEXT("not find this arg type"));
}

template <class T>
void FLuaUtil::pop(FClassType<T> &&ReturnValue)
{
	const char *ClassName = ReturnValue.m_ClassName;
	if (!ExistClass(ReturnValue.m_ClassName))
	{
		LuaWrapperLog(Fatal, TEXT("can not pop class %s, it not export!!!"), ReturnValue.m_ClassName);
		return;
	}

	TouserCppClassType(ReturnValue);
}

template <class T1, class... T>
void FLuaUtil::CallR( T1 &Value, T&&... args)
{
	CallRImpl(FLuaReturnTypeNum(0), Forward<T>(args)...);
}

template <class T1, class... T>
void FLuaUtil::CallRImpl(FLuaReturnTypeNum &&RetTypeNum, T1 &&ReturnValue, T&&... args)
{
	CallRImpl(RetTypeNum+1, Forward<T>(args)...);
	pop(ReturnValue);
}

template <class... T>
void FLuaUtil::CallRImpl(FLuaReturnTypeNum &&RetTypeNum, const FLuaFuncName &Value, T&&... args)
{
	lua_pushcfunction(g_LuaState, LuaErrHandleFunc);
	lua_getfield(g_LuaState, LUA_GLOBALSINDEX, Value.m_FuncName);
	int32 paramCount = Push(Forward<T>(args)...);
	if (lua_pcall(g_LuaState, paramCount, RetTypeNum.m_num, -(paramCount + 2)))
	{
		LuaWrapperLog(Fatal, TEXT("call r impl found an error: %s"), lua_tostring(g_LuaState, -1)));
	}
}

template <class T>
void FLuaUtil::TouserCppClassType(FClassType<T> &&OutValue)
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
		LuaWrapperLog(Error, TEXT("TouserCppClassType error"));
	}
}

