#pragma once
#include "IScriptGenerator.h"
#include "BaseLuaFuncReg.h"

class FUStructGenerator : public IScriptGenerator
{
public:
	static IScriptGenerator* CreateGenerator(UScriptStruct *InScriptStruct, const FString &InOutDir);

public:
	FUStructGenerator(UScriptStruct *InScriptStruct, const FString &InOutDir);
	virtual ~FUStructGenerator();

public:
	/** FBaseGenerator interface */
	virtual FString GetKey() const override;
	virtual bool CanExport()const  override;
	virtual void ExportToMemory() override;
	virtual void SaveToFile() override;
	virtual FString GetClassName() const override;

private:
	void ExportDataMemberToMemory();
	void ExportFunctionsToMemory();
	void ExportExtraFuncsToMemory();

	bool CanExportFunction(UFunction *InFunction);
	bool CanExportProperty(UProperty *InProperty);

	FString GetFuncContents();
	FString GetRegContents();

private:
	FExtraFuncMemberInfo GenerateNewExportFunction();
	FExtraFuncMemberInfo GenerateDestoryExportFunction();

private:
	UScriptStruct *m_pScriptStruct;
	FBaseFuncReg m_LuaFuncReg;
};

