#pragma once
#include "IScriptGenerator.h"

class FClassGeneratorConfig
{
public:
	void Init();
	bool CanExport(const FString &InClassName);

private:
	TArray<FString> m_NotSuportClasses;
};

class FClassGenerator : public IScriptGenerator
{
public:
	static IScriptGenerator* CreateGenerator(UObject *InObj, const FString &OutDir);

public:
	FClassGenerator(UClass *InClass, const FString &InOutDir);
	virtual ~FClassGenerator();

public:
	/** FBaseGenerator interface */
	virtual FString GetKey() override { return m_FileName; }
	virtual bool CanExport() override;
	virtual void Export() override;
	virtual void SaveToFile() override;

private:
	void GenerateScriptHeader(FString &OutStr);
	void GenerateFunctions(FString &OutStr);
	void GenerateRegister(FString &OutStr);
	void GenerateScriptTail(FString &OutStr);

private:
	void GenerateRegisterItem(const FString &InFunctionName, FString &OutStr);
	FString GenerateRegisterFuncName(const FString &InFunctionName, const FString &ClassName);
	bool CanExportFunction(UFunction *InFunction);
	void GenerateSingleFunction(UFunction *InFunction, FString &OutStr);
	void AddFunctionToRegister(UFunction *InFunction);
	void GenerateFunctionParam(UProperty *InParam, int32 InIndex, FString &OutStr);
	void GenerateFunctionParams(UFunction *InFunction, FString &OutStr);
	void GeneratorCheckParamsNumCode(UFunction *InFunction, FString &OutStr);
	FString GetPropertyType(UProperty *InParam, uint32 PortFlags=0);

private:
	UClass *m_pClass;
	FString m_OutDir;
	FString m_FileName;
	FString m_FileContent;
	TArray<FString> m_FunctionNames;
	
public:
	static FClassGeneratorConfig m_ClassConfig;
};
