#pragma once
#include "IScriptGenerator.h"




class FConfigClassGenerator : public IScriptGenerator
{
public:
	static IScriptGenerator* CreateGenerator(UObject *InObj, const FString &OutDir);

public:

};