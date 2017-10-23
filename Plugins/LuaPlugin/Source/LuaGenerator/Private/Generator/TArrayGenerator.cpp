#include "Generator/TArrayGenerator.h"
#include "GeneratorDefine.h"
using namespace NS_LuaGenerator;

IScriptGenerator* FTArrayGenerator::CreateGenerator( UProperty *pElementProperty, const FString &InOutDir)
{
	return new FTArrayGenerator(pElementProperty, InOutDir);
}

FTArrayGenerator::FTArrayGenerator(UProperty *pElementProperty, const FString &InOutDir)
	: IScriptGenerator(E_GeneratorType::ETArray, InOutDir)
{
	m_ElementInfo.InitByUProperty(pElementProperty);
	m_LuaFuncReg.Init();
}

FTArrayGenerator::~FTArrayGenerator()
{

}

FString FTArrayGenerator::GetKey() const
{
	return FString::Printf(TEXT("TArray_%s"), *m_ElementName)
}

void FTArrayGenerator::InitElementName(UProperty *pProperty)
{
	m_bSupportElement = false;
	EVariableType  ePropertyType = ResolvePropertyType(pProperty);
	FString InnerType = GetPropertyType(pProperty)

	switch (ePropertyType)
	{
	case EBaseType:
	{
		m_bSupportElement = true;
		m_ElementName = InnerType;
		break;
	}
	case EPoint:
	{
		m_bSupportElement = true;
		m_ElementName = ReplaceStarWithPoint(InnerType);
		break;
	}
	case EObjectBase:
	{
		m_bSupportElement = false;
		break;
	}
	case EFName:
	{
		
		break;
	}
	case EText:
	{
	
		break;
	}
	case EFString:
	{
		
		break;
	}
	case EClass:
	{
		
		break;
	}
	case ETArray:
	{
		
		break;
	}
	case EWeakObject:
	{
		
		break;
	}
	case EStruct:
	{
		
		break;
	}
	case EByte:
	{
		
		break;
	}
	case EEnum:
	{
		
		break;
	}
	case ETSubclassOf:
	{
		
		break;
	}
	case EMulticastDelegate:
	{
	
		break;
	}
	case ETMap:
	{
	
		break;
	}
	case ETSet:
	{
		
		break;
	}
	case EUnknow:
	{
		m_bSupportElement = false;
		break;
	}
	default:
	{

	}
	}


}

FString FTArrayGenerator::ReplaceStarWithPoint(const FString &InStr)
{
	FString Ret;
	for (int32 i=0; i<InStr.Len(); ++i)
	{
		if (InStr[i]=='*')
		{
			Ret += "Point";
		}
		else
		{
			Ret.AppendChar(InStr[i]);
		}
	}

	return Ret;
}

