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
	virtual bool CanExport() const override;
	virtual void Export() override;
	virtual void SaveToFile() override;

private:
	void GeneratorScriptName(FString &OutStr);
	void GenerateScriptHeader(FString &OutScriptHeader);
	void GenerateFunctions(TMap<FString, FString> &OutFunctionMap);
	void GenerateFunctionKey(const FConfigFunction &FuncItem, FString &OutKey);
	void GenerateFunctionValue(const FConfigFunction &FuncItem, FString &OutStr);
	void GenerateRegVariableHeader(FString &OutStr);
	void GenerateRegVariableItems(TMap<FString, FString> &OutMap);
	void GenerateRegVariableItemKey(const FConfigFunction &FuncItem, FString &OutStr);
	void GenerateRegVariableItemValue(const FConfigFunction &FuncItem, FString &OutStr);
	void GenerateRegVariableTail(FString &OutStr);
	void GenerateScriptTail(FString &OutStr);

private:
	void AddToMap(const char *MapType, const FString &InKey, const FString &InValue, TMap<FString, FString> &InMap );
	void GeneratorFunctionName(const FConfigFunction &FuncItem, FString &OutStr);

private:
	void Unity(FString &OutStr);

private:
	FString m_FileName;
	FString m_ScriptHeader;
	TMap<FString, FString> m_Functions;
	FString m_RegVariableHeader;
	TMap<FString, FString> m_RegVariableItems;
	FString m_RegVariableTail;
	FString m_ScriptTail;
	FConfigClass m_ConfigClass;
};