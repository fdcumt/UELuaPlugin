#pragma once
#include "IScriptGenerator.h"
#include "BaseLuaFuncReg.h"

class FTSetGenerator : public IScriptGenerator
{
public:
	static IScriptGenerator* CreateGenerator(UProperty *pProperty, const FString &InOutDir);

public:
	FTSetGenerator(UProperty *pProperty, const FString &InOutDir);
	virtual ~FTSetGenerator();

public:
	/** FBaseGenerator interface */
	virtual FString GetKey() const override;
	virtual bool CanExport()const  override;
	virtual void ExportToMemory() override;
	virtual void SaveToFile() override;
	virtual FString GetClassName() const override;

private:
	FExtraFuncMemberInfo ExtraNum();
	FExtraFuncMemberInfo ExtraAdd();
	FExtraFuncMemberInfo ExtraEmpty();
	FExtraFuncMemberInfo ExtraRemove();
	FExtraFuncMemberInfo ExtraContains();

private:
	void Init(USetProperty *pSetProperty);

private:
	FString m_ClassName;
	bool m_bSupportElement;
	FBaseFuncReg m_LuaFuncReg;
	FVariableTypeInfo m_ElementInfo;
	FVariableTypeInfo m_TSetInfo;
};

