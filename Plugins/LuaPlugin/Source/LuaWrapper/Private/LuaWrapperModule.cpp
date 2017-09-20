#include "LuaWrapperModule.h"
#include "LuaWrapper.h"
#include "LuaWrapperDefine.h"

#define LOCTEXT_NAMESPACE "FLuaWrapperModule"

void FLuaWrapperModule::StartupModule()
{
	
}

void FLuaWrapperModule::ShutdownModule()
{
	delete m_LuaWrapper;
}

void FLuaWrapperModule::Restart()
{
	m_LuaWrapper->Restart();
}


void FLuaWrapperModule::Init()
{
	g_LuaWrapper = m_LuaWrapper = new FLuaWrapper();
	m_LuaWrapper->Init();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FLuaWrapperModule, LuaWrapper)


