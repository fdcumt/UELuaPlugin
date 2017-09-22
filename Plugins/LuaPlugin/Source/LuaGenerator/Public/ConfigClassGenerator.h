#pragma once
#include "GeneratorDefine.h"
#include "IScriptGenerator.h"

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
	void GeneratorScriptName(FString &OutStr);
	void GenerateScriptHeader(FString &OutScriptHeader);
	void GeneratorIncludeHeaders(TArray<FString> &OutStr);
	void GeneratorIncludeHeader(const FString &IncludeFile, FString &OutStr);
	void GenerateDisableWarning(FString &OutStr);
	void GenerateFunctions(TMap<FString, FString> &OutFunctionMap);
	void GenerateFunctionKey(const FConfigFunction &FuncItem, FString &OutKey);
	void GenerateFunctionValue(const FConfigFunction &FuncItem, FString &OutStr);
	void GenerateRegVariableHeader(FString &OutStr);
	void GenerateRegVariableItems(TMap<FString, FString> &OutMap);
	void GenerateRegVariableItemKey(const FConfigFunction &FuncItem, FString &OutStr);
	void GenerateRegVariableItemValue(const FConfigFunction &FuncItem, FString &OutStr);
	void GenerateRegVariableTail(FString &OutStr);
	void GenerateEnableWarning(FString &OutStr);
	void GenerateScriptTail(FString &OutStr);

private:
	void AddToMap(const char *MapType, const FString &InKey, const FString &InValue, TMap<FString, FString> &InMap );
	void GeneratorFunctionName(const FConfigFunction &FuncItem, FString &OutStr);
	FString GenerateRegVariableName() const ;

private:
	void Unity(FString &OutStr);

private:
	FString m_FileName;
	FString m_ScriptHeader;
	TArray<FString> m_IncludeFiles;
	FString m_DisableWarning;
	TMap<FString, FString> m_Functions;
	FString m_RegVariableHeader;
	TMap<FString, FString> m_RegVariableItems;
	FString m_RegVariableTail;
	FString m_EnableWarning;
	FString m_ScriptTail;
	FConfigClass m_ConfigClass;
};