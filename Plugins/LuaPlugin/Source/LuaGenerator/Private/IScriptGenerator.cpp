#include "IScriptGenerator.h"

IScriptGenerator::IScriptGenerator(NS_LuaGenerator::E_GeneratorType InType, const FString &OutDir)
	: m_eClassType(InType)
	, m_OutDir(OutDir)
{

}

IScriptGenerator::~IScriptGenerator()
{

}

