#pragma once
#include "IScriptGenerator.h"
#include "BaseLuaFuncReg.h"

class FTArrayGenerator : public IScriptGenerator
{
public:
	static IScriptGenerator* CreateGenerator(const FString &InArrayElementType, const FString &InOutDir);

public:
	FTArrayGenerator(UProperty *pElementProperty, const FString &InOutDir);
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
	FExtraFuncMemberInfo ExtraFind();
	FExtraFuncMemberInfo ExtraGet();
	FExtraFuncMemberInfo ExtraSet();
	FExtraFuncMemberInfo ExtraClear();
	FExtraFuncMemberInfo ExtraRemove();

private:
	void InitElementName(UProperty *pProperty);
	FString ReplaceStarWithPoint(const FString &InStr);

private:
	bool m_bSupportElement;
	FString m_ElementName;
	FVariableTypeInfo m_ElementInfo;
	FBaseFuncReg m_LuaFuncReg;
};

