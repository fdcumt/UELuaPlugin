#pragma once
#include "GeneratorDefine.h"

class IScriptGenerator
{
public:
	IScriptGenerator(NS_LuaGenerator::E_GeneratorType InType, const FString &OutDir);
	virtual ~IScriptGenerator();

public:
	virtual bool CanExport() const = 0;
	virtual void ExportToMemory() = 0;
	virtual void SaveToFile() = 0;

public:
	virtual FString GetKey() const = 0;
	virtual FString GetClassName() const = 0;
	virtual FString GetFileName() const = 0;
	virtual FString GetRegName() const = 0;
	virtual void GetParentNames(TArray<FString> &OutParentNames) const = 0;

public:
	virtual FString GetFileHeader();
	virtual FString GetFileTail();

public:
	NS_LuaGenerator::E_GeneratorType GetType() const { return m_eClassType; };

protected:
	NS_LuaGenerator::E_GeneratorType m_eClassType;
	FString m_OutDir;
};
