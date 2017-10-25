#include "Generator/TSetGenerator.h"
#include "CoreUObject.h"
#include "GeneratorDefine.h"
#include "LuaConfigManager.h"
using namespace NS_LuaGenerator;

IScriptGenerator* FTSetGenerator::CreateGenerator(UProperty *pProperty, const FString &InOutDir)
{
	return new FTSetGenerator(pProperty, InOutDir);
}

FTSetGenerator::FTSetGenerator(UProperty *pProperty, const FString &InOutDir)
	: IScriptGenerator(E_GeneratorType::ETSet, InOutDir)
{
	USetProperty *pSetProperty = Cast<USetProperty>(pProperty);
	Init(pSetProperty);
}

FTSetGenerator::~FTSetGenerator()
{
	
}

FString FTSetGenerator::GetKey() const
{
	return m_TSetInfo.PureType;
}

bool FTSetGenerator::CanExport() const
{
	return m_bSupportElement;
}

void FTSetGenerator::ExportToMemory()
{
	m_LuaFuncReg.AddExtraFuncMember(ExtraNum());
	m_LuaFuncReg.AddExtraFuncMember(ExtraAdd());
	m_LuaFuncReg.AddExtraFuncMember(ExtraEmpty());
	m_LuaFuncReg.AddExtraFuncMember(ExtraRemove());
	m_LuaFuncReg.AddExtraFuncMember(ExtraContains());
}

void FTSetGenerator::SaveToFile()
{
	FString FileContent;
	FString FilePathName = m_OutDir / GetFileName();
	FileContent += GetFileHeader();
	FileContent += m_LuaFuncReg.GetFuncContents();
	FileContent += m_LuaFuncReg.GetRegLibContents();
	FileContent += GetFileTail();

	if (!FFileHelper::SaveStringToFile(FileContent, *FilePathName))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("Failed to save header export:%s"), *GetFileName());
	}
}

FString FTSetGenerator::GetClassName() const
{
	return m_ClassName;
}

FExtraFuncMemberInfo FTSetGenerator::ExtraNum()
{
	FExtraFuncMemberInfo ExtraInfo;
	ExtraInfo.funcName = "Num";
	FString &funcBody = ExtraInfo.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pTSet = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_TSetInfo.PureType, *m_TSetInfo.PureType, *m_TSetInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\tint32 TSetNum = pTSet->Num();"));
	funcBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<int32>(TSetNum, \"int32\"));"));
	funcBody += EndLinePrintf(TEXT("\treturn 1;"));
	return ExtraInfo;
}

FExtraFuncMemberInfo FTSetGenerator::ExtraAdd()
{
	FExtraFuncMemberInfo ExtraInfo;
	ExtraInfo.funcName = "Add";
	FString &funcBody = ExtraInfo.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pTSet = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_TSetInfo.PureType, *m_TSetInfo.PureType, *m_TSetInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\t%s ElementInfo = %sFLuaUtil::TouserData<%s>(InLuaState, 2, \"%s\");"), *m_ElementInfo.DeclareType, *m_ElementInfo.CastType, *m_ElementInfo.TouserPushDeclareType, *m_ElementInfo.TouserPushPureType);
	funcBody += EndLinePrintf(TEXT("\tpTSet->Add(%sElementInfo);"), *m_ElementInfo.UsedSelfVarPrefix);
	funcBody += EndLinePrintf(TEXT("\treturn 0;"));
	return ExtraInfo;
}

FExtraFuncMemberInfo FTSetGenerator::ExtraEmpty()
{
	FExtraFuncMemberInfo ExtraInfo;
	ExtraInfo.funcName = "Empty";
	FString &funcBody = ExtraInfo.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pTSet = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_TSetInfo.PureType, *m_TSetInfo.PureType, *m_TSetInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\tpTSet->Empty();"));
	funcBody += EndLinePrintf(TEXT("\treturn 0;"));
	return ExtraInfo;
}

FExtraFuncMemberInfo FTSetGenerator::ExtraRemove()
{
	FExtraFuncMemberInfo ExtraInfo;
	ExtraInfo.funcName = "Remove";
	FString &funcBody = ExtraInfo.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pTSet = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_TSetInfo.PureType, *m_TSetInfo.PureType, *m_TSetInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\t%s ElementInfo = %sFLuaUtil::TouserData<%s>(InLuaState, 2, \"%s\");"), *m_ElementInfo.DeclareType, *m_ElementInfo.CastType, *m_ElementInfo.TouserPushDeclareType, *m_ElementInfo.TouserPushPureType);
	funcBody += EndLinePrintf(TEXT("\tpTSet->Remove(%sElementInfo);"), *m_ElementInfo.UsedSelfVarPrefix);
	funcBody += EndLinePrintf(TEXT("\treturn 0;"));
	return ExtraInfo;
}

FExtraFuncMemberInfo FTSetGenerator::ExtraContains()
{
	FExtraFuncMemberInfo ExtraInfo;
	ExtraInfo.funcName = "Contains";
	FString &funcBody = ExtraInfo.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pTSet = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_TSetInfo.PureType, *m_TSetInfo.PureType, *m_TSetInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\t%s ElementInfo = %sFLuaUtil::TouserData<%s>(InLuaState, 2, \"%s\");"), *m_ElementInfo.DeclareType, *m_ElementInfo.CastType, *m_ElementInfo.TouserPushDeclareType, *m_ElementInfo.TouserPushPureType);
	funcBody += EndLinePrintf(TEXT("\tbool bContain = pTSet->Contains(%sElementInfo);"), *m_ElementInfo.UsedSelfVarPrefix);
	funcBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<bool>(bContain, \"bool\"));"));
	funcBody += EndLinePrintf(TEXT("\treturn 1;"));
	return ExtraInfo;
}

void FTSetGenerator::Init(USetProperty *pSetProperty)
{
	m_bSupportElement = false;
	m_ClassName = NS_LuaGenerator::GetPlainType(NS_LuaGenerator::GetPropertyType(pSetProperty));
	m_LuaFuncReg.Init(m_ClassName);
	m_TSetInfo.InitByUProperty(pSetProperty);
	m_ElementInfo.InitByUProperty(pSetProperty->ElementProp);

	switch (m_ElementInfo.eVariableType)
	{
	case EBaseType:
	{
		m_bSupportElement = true;
		break;
	}
	case EPoint:
	{
		m_bSupportElement = true;
		break;
	}
	case EObjectBase:
	{
		m_bSupportElement = false;
		break;
	}
	case EFName:
	{
		m_bSupportElement = true;
		break;
	}
	case EText:
	{
		m_bSupportElement = true;
		break;
	}
	case EFString:
	{
		m_bSupportElement = true;
		break;
	}
	case EClass:
	{
		m_bSupportElement = true;
		break;
	}
	case EVarTArray:
	{
		m_bSupportElement = true;
		break;
	}
	case EWeakObject:
	{
		m_bSupportElement = false;
		break;
	}
	case EStruct:
	{
		m_bSupportElement = true;
		break;
	}
	case EByte:
	{
		m_bSupportElement = false;
		break;
	}
	case EEnum:
	{
		m_bSupportElement = false;
		break;
	}
	case ETSubclassOf:
	{
		m_bSupportElement = false;
		break;
	}
	case EMulticastDelegate:
	{
		m_bSupportElement = false;
		break;
	}
	case EVarTMap:
	{
		m_bSupportElement = true;
		break;
	}
	case EVarTSet:
	{
		m_bSupportElement = true;
		break;
	}
	case EUnknow:
	{
		m_bSupportElement = false;
		break;
	}
	}
}

