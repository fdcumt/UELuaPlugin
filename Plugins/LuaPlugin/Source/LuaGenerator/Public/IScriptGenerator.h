#pragma once
#include "GeneratorDefine.h"

class IScriptGenerator
{
public:
	IScriptGenerator(NS_LuaGenerator::E_GeneratorType InType, const FString &OutDir);
	virtual ~IScriptGenerator();

public:
	virtual FString GetKey() const = 0;
	virtual FString GetFileName() const = 0;
	virtual FString GetRegName() const = 0;
	virtual bool CanExport() const = 0;
	virtual void ExportToMemory() = 0;
	virtual void SaveToFile() = 0;

public:
	NS_LuaGenerator::E_GeneratorType GetType() const { return m_Type; };

protected:
	NS_LuaGenerator::E_GeneratorType m_Type;
	FString m_OutDir;
};
