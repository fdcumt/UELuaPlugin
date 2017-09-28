#pragma once
#include "GeneratorDefine.h"
#include "IScriptGenerator.h"
#include "ConfigClassDefine.h"

class FConfigClassGenerator : public IScriptGenerator
{
public:
	static IScriptGenerator* CreateGenerator(const FConfigClass& ClassItem, const FString &OutDir);

public:
	FConfigClassGenerator(const FConfigClass &ConfigClass, const FString &OutDir);
	virtual ~FConfigClassGenerator();

public:
	/** FBaseGenerator interface */
	virtual FString GetKey() const override;
	virtual FString GetFileName() const override;
	virtual FString GetRegName() const override;
	virtual bool CanExport() const override;
	virtual void ExportToMemory() override;
	virtual void SaveToFile() override;

private:
	void Unity(FString &OutStr);

private:
	FConfigClass m_ConfigClass;
};