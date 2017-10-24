#include "Generator/TMapGenerator.h"
#include "CoreUObject.h"
#include "GeneratorDefine.h"
#include "LuaConfigManager.h"
using namespace NS_LuaGenerator;

IScriptGenerator* FTMapGenerator::CreateGenerator(UProperty *pProperty, const FString &InOutDir)
{
	return new FTMapGenerator(pProperty, InOutDir);
}

FTMapGenerator::FTMapGenerator(UProperty *pProperty, const FString &InOutDir)
	: IScriptGenerator(E_GeneratorType::ETMap, InOutDir)
{
	UMapProperty *pMapProperty = Cast<UMapProperty>(pProperty);
	Init(pMapProperty);
}

FTMapGenerator::~FTMapGenerator()
{

}

FString FTMapGenerator::GetKey() const
{
	return m_TMapInfo.PureType;
}

FString FTMapGenerator::GetFileName() const
{
	return GetClassName() + g_LuaConfigManager->ClassScriptHeaderSuffix;
}

FString FTMapGenerator::GetRegName() const
{
	return FString::Printf(TEXT("%s_Lib"), *GetClassName());
}

bool FTMapGenerator::CanExport() const
{
	return m_bSupportKey && m_bSupportValue;
}

void FTMapGenerator::ExportToMemory()
{
	m_LuaFuncReg.AddExtraFuncMember(ExtraNum());
	m_LuaFuncReg.AddExtraFuncMember(ExtraAdd());
	m_LuaFuncReg.AddExtraFuncMember(ExtraFind());
	m_LuaFuncReg.AddExtraFuncMember(ExtraContains());
	m_LuaFuncReg.AddExtraFuncMember(ExtraEmpty());
	m_LuaFuncReg.AddExtraFuncMember(ExtraRemove());
}

void FTMapGenerator::SaveToFile()
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

FString FTMapGenerator::GetClassName() const
{
	return FString::Printf(TEXT("%s"), *m_ClassName);
}

void FTMapGenerator::GetParentNames(TArray<FString> &OutParentNames) const
{

}

FExtraFuncMemberInfo FTMapGenerator::ExtraNum()
{
	FExtraFuncMemberInfo ExtraInfo;
	ExtraInfo.funcName = "Num";
	FString &funcBody = ExtraInfo.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pMap = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_TMapInfo.PureType, *m_TMapInfo.PureType, *m_TMapInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\tint32 MapNum = pMap->Num();"));
	funcBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<int32>(MapNum, \"int32\"));"));
	funcBody += EndLinePrintf(TEXT("\treturn 1;"));
	return ExtraInfo;
}

FExtraFuncMemberInfo FTMapGenerator::ExtraAdd()
{
	FExtraFuncMemberInfo ExtraInfo;
	ExtraInfo.funcName = "Add";
	FString &funcBody = ExtraInfo.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pMap = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_TMapInfo.PureType, *m_TMapInfo.PureType, *m_TMapInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\t%s MapKey = FLuaUtil::TouserData<%s>(InLuaState, 2, \"%s\");"), *m_KeyInfo.DeclareType, *m_KeyInfo.DeclareType, *m_KeyInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\t%s MapValue = FLuaUtil::TouserData<%s>(InLuaState, 3, \"%s\");"), *m_ValueInfo.DeclareType, *m_ValueInfo.DeclareType, *m_ValueInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\tpMap->Add(%sMapKey, %sMapValue);"), *m_KeyInfo.UsedSelfVarPrefix, *m_ValueInfo.UsedSelfVarPrefix);
	funcBody += EndLinePrintf(TEXT("\treturn 0;"));
	return ExtraInfo;
}

FExtraFuncMemberInfo FTMapGenerator::ExtraFind()
{
	FExtraFuncMemberInfo ExtraInfo;
	ExtraInfo.funcName = "Find";
	FString &funcBody = ExtraInfo.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pMap = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_TMapInfo.PureType, *m_TMapInfo.PureType, *m_TMapInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\t%s MapKey = FLuaUtil::TouserData<%s>(InLuaState, 2, \"%s\");"), *m_KeyInfo.DeclareType, *m_KeyInfo.DeclareType, *m_KeyInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\t%s pMapValue = pMap->Find(%sMapKey);"), *m_ValueInfo.PointTValueDeclare, *m_KeyInfo.UsedSelfVarPrefix);
	funcBody += EndLinePrintf(TEXT("\tif (pMapValue)"));
	funcBody += EndLinePrintf(TEXT("\t{"));
	funcBody += EndLinePrintf(TEXT("\t\tFLuaUtil::Push(InLuaState, FLuaClassType<%s>(%spMapValue, \"%s\"));"), *m_ValueInfo.DeclareType, *m_ValueInfo.PushPointTValuePrefix, *m_ValueInfo.PureType );
	funcBody += EndLinePrintf(TEXT("\t}"));
	funcBody += EndLinePrintf(TEXT("\telse"));
	funcBody += EndLinePrintf(TEXT("\t{"));
	funcBody += EndLinePrintf(TEXT("\t\tFLuaUtil::PushNil(InLuaState);"), *m_ValueInfo.DeclareType, *m_ValueInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\t}"));
	funcBody += EndLinePrintf(TEXT("\treturn 1;"));
	return ExtraInfo;
}

FExtraFuncMemberInfo FTMapGenerator::ExtraContains()
{
	FExtraFuncMemberInfo ExtraInfo;
	ExtraInfo.funcName = "Contains";
	FString &funcBody = ExtraInfo.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pMap = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_TMapInfo.PureType, *m_TMapInfo.PureType, *m_TMapInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\t%s MapKey = FLuaUtil::TouserData<%s>(InLuaState, 2, \"%s\");"), *m_KeyInfo.DeclareType, *m_KeyInfo.DeclareType, *m_KeyInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\tbool bContain = pMap->Contains(%sMapKey);"), *m_KeyInfo.UsedSelfVarPrefix);
	funcBody += EndLinePrintf(TEXT("\tFLuaUtil::Push(InLuaState, FLuaClassType<bool>(bContain, \"bool\"));"));
	funcBody += EndLinePrintf(TEXT("\treturn 1;"));
	return ExtraInfo;
}

FExtraFuncMemberInfo FTMapGenerator::ExtraEmpty()
{
	FExtraFuncMemberInfo ExtraInfo;
	ExtraInfo.funcName = "Empty";
	FString &funcBody = ExtraInfo.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pMap = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_TMapInfo.PureType, *m_TMapInfo.PureType, *m_TMapInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\tpMap->Empty();"));
	funcBody += EndLinePrintf(TEXT("\treturn 0;"));
	return ExtraInfo;
}

FExtraFuncMemberInfo FTMapGenerator::ExtraRemove()
{
	FExtraFuncMemberInfo ExtraInfo;
	ExtraInfo.funcName = "Remove";
	FString &funcBody = ExtraInfo.funcBody;
	funcBody += EndLinePrintf(TEXT("\t%s *pMap = FLuaUtil::TouserData<%s*>(InLuaState, 1, \"%s\");"), *m_TMapInfo.PureType, *m_TMapInfo.PureType, *m_TMapInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\t%s MapKey = FLuaUtil::TouserData<%s>(InLuaState, 2, \"%s\");"), *m_KeyInfo.DeclareType, *m_KeyInfo.DeclareType, *m_KeyInfo.PureType);
	funcBody += EndLinePrintf(TEXT("\tpMap->Remove(%sMapKey);"), *m_KeyInfo.UsedSelfVarPrefix);
	funcBody += EndLinePrintf(TEXT("\treturn 0;"));
	return ExtraInfo;
}

void FTMapGenerator::Init(UMapProperty *pMapProperty)
{
	m_bSupportKey = false;
	m_bSupportValue = false;
	m_ClassName = NS_LuaGenerator::GetPlainType(NS_LuaGenerator::GetPropertyType(pMapProperty));
	m_LuaFuncReg.Init(m_ClassName);
	m_TMapInfo.InitByUProperty(pMapProperty);
	m_KeyInfo.InitByUProperty(pMapProperty->KeyProp);
	m_ValueInfo.InitByUProperty(pMapProperty->ValueProp);

	switch (m_KeyInfo.eVariableType)
	{
	case EBaseType:
	{
		m_bSupportKey = true;
		break;
	}
	case EPoint:
	{
		m_bSupportKey = true;
		break;
	}
	case EObjectBase:
	{
		m_bSupportKey = false;
		break;
	}
	case EFName:
	{
		m_bSupportKey = true;
		break;
	}
	case EText:
	{
		m_bSupportKey = true;
		break;
	}
	case EFString:
	{
		m_bSupportKey = true;
		break;
	}
	case EClass:
	{
		m_bSupportKey = true;
		break;
	}
	case EVarTArray:
	{
		m_bSupportKey = true;
		break;
	}
	case EWeakObject:
	{
		m_bSupportKey = false;
		break;
	}
	case EStruct:
	{
		m_bSupportKey = true;
		break;
	}
	case EByte:
	{
		m_bSupportKey = false;
		break;
	}
	case EEnum:
	{
		m_bSupportKey = false;
		break;
	}
	case ETSubclassOf:
	{
		m_bSupportKey = false;
		break;
	}
	case EMulticastDelegate:
	{
		m_bSupportKey = false;
		break;
	}
	case EVarTMap:
	{
		m_bSupportKey = true;
		break;
	}
	case EVarTSet:
	{
		m_bSupportKey = false;
		break;
	}
	case EUnknow:
	{
		m_bSupportKey = false;
		break;
	}
	}

	switch (m_ValueInfo.eVariableType)
	{
	case EBaseType:
	{
		m_bSupportValue = true;
		break;
	}
	case EPoint:
	{
		m_bSupportValue = true;
		break;
	}
	case EObjectBase:
	{
		m_bSupportValue = false;
		break;
	}
	case EFName:
	{
		m_bSupportValue = true;
		break;
	}
	case EText:
	{
		m_bSupportValue = true;
		break;
	}
	case EFString:
	{
		m_bSupportValue = true;
		break;
	}
	case EClass:
	{
		m_bSupportValue = true;
		break;
	}
	case EVarTArray:
	{
		m_bSupportValue = true;
		break;
	}
	case EWeakObject:
	{
		m_bSupportValue = false;
		break;
	}
	case EStruct:
	{
		m_bSupportValue = true;
		break;
	}
	case EByte:
	{
		m_bSupportValue = false;
		break;
	}
	case EEnum:
	{
		m_bSupportValue = false;
		break;
	}
	case ETSubclassOf:
	{
		m_bSupportValue = false;
		break;
	}
	case EMulticastDelegate:
	{
		m_bSupportValue = false;
		break;
	}
	case EVarTMap:
	{
		m_bSupportValue = true;
		break;
	}
	case EVarTSet:
	{
		m_bSupportValue = false;
		break;
	}
	case EUnknow:
	{
		m_bSupportValue = false;
		break;
	}
	}
}
