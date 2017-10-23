#pragma once
#include "IScriptGenerator.h"
#include "BaseLuaFuncReg.h"

class FTArrayGenerator : public IScriptGenerator
{
public:
	static IScriptGenerator* CreateGenerator(UProperty *pProperty, const FString &InOutDir);

public:
	FTArrayGenerator(UProperty *pProperty, const FString &InOutDir);
	virtual ~FTArrayGenerator();

public:
	/** FBaseGenerator interface */
	virtual FString GetKey() const override;
	virtual FString GetFileName() const override;
	virtual FString GetRegName() const override;
	virtual bool CanExport()const  override;
	virtual void ExportToMemory() override;
	virtual void SaveToFile() override;
	virtual FString GetClassName() const override;
	virtual void GetParentNames(TArray<FString> &OutParentNames) const;

private:
	FExtraFuncMemberInfo ExtraNew();
	FExtraFuncMemberInfo ExtraDestory();
	FExtraFuncMemberInfo ExtraNum();
	FExtraFuncMemberInfo ExtraAdd();
	FExtraFuncMemberInfo ExtraGet();
	FExtraFuncMemberInfo ExtraSet();
	FExtraFuncMemberInfo ExtraEmpty();
	FExtraFuncMemberInfo ExtraCopy();
	FExtraFuncMemberInfo ExtraRemoveAt();

private:
	void Init(UArrayProperty *pArrayProperty);
	
private:
	FString m_ClassName;
	bool m_bSupportElement;
	FBaseFuncReg m_LuaFuncReg;
	FVariableTypeInfo m_ElementInfo;
	FVariableTypeInfo m_TArrayInfo;
};

