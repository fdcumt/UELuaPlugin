#include "LuaWrapperModule.h"
#include "LuaWrapper.h"
#include "LuaWrapperDefine.h"

#define LOCTEXT_NAMESPACE "FLuaWrapperModule"

void FLuaWrapperModule::StartupModule()
{
	g_LuaWrapper = m_LuaWrapper = new FLuaWrapper();
}

void FLuaWrapperModule::ShutdownModule()
{
	delete m_LuaWrapper;
}

void FLuaWrapperModule::Restart()
{
	m_LuaWrapper->Restart();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FLuaWrapperModule, LuaWrapper)


