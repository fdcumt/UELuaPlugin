#pragma once
#include "IScriptGenerator.h"
#include "BaseLuaFuncReg.h"

class FTMapGenerator : public IScriptGenerator
{
public:
	static IScriptGenerator* CreateGenerator(UProperty *pProperty, const FString &InOutDir);

public:
	FTMapGenerator(UProperty *pProperty, const FString &InOutDir);
	virtual ~FTMapGenerator();

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
	FExtraFuncMemberInfo ExtraNum();
	FExtraFuncMemberInfo ExtraAdd();
	FExtraFuncMemberInfo ExtraFind();
	FExtraFuncMemberInfo ExtraContains();
	FExtraFuncMemberInfo ExtraEmpty();
	FExtraFuncMemberInfo ExtraRemove();

private:
	void Init(UMapProperty *pMapProperty);

private:
	FString m_ClassName;
	bool m_bSupportKey;
	bool m_bSupportValue;
	FBaseFuncReg m_LuaFuncReg;
	FVariableTypeInfo m_KeyInfo;
	FVariableTypeInfo m_ValueInfo;
	FVariableTypeInfo m_TMapInfo;
};
