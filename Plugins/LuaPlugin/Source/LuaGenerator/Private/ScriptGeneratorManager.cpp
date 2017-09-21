#include "ScriptGeneratorManager.h"
#include "UnrealType.h"
#include "ClassGenerator.h"
#include "GeneratorDefine.h"
#include "Misc/FileHelper.h"
#include "UObjectIterator.h"
#include "Serialization/JsonSerializer.h"

FScriptGeneratorManager::FScriptGeneratorManager()
{

}

FScriptGeneratorManager::~FScriptGeneratorManager()
{

}

void FScriptGeneratorManager::Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase)
{
	m_RootLocalPath = RootLocalPath;
	m_RootBuildPath = RootBuildPath;
	m_OutputDirectory = OutputDirectory;
	m_IncludeBase = IncludeBase;

	//DebugLog(TEXT("m_RootLocalPath: %s"), *m_RootLocalPath);
	//DebugLog(TEXT("m_RootBuildPath: %s"), *m_RootBuildPath);
	//DebugLog(TEXT("m_OutputDirectory: %s"), *m_OutputDirectory);
}

void FScriptGeneratorManager::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	IScriptGenerator *pGenerator = CreateGeneratorByType(NS_LuaGenerator::EClass, Class);

	if (!CanExportClass(pGenerator))
	{
		return;
	}
	
	pGenerator->Export();
}

void FScriptGeneratorManager::FinishExport()
{
	ExportConfigClass();
}

IScriptGenerator* FScriptGeneratorManager::CreateGeneratorByType(NS_LuaGenerator::E_GeneratorType InType, UObject *InObj)
{
	switch (InType)
	{
	case NS_LuaGenerator::EClass:
	{
		return FClassGenerator::CreateGenerator(InObj, m_OutputDirectory);
	}
	default:
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("FScriptGeneratorManager::CreateGeneratorByType error, not found generator type:%d"), int32(InType));
	}
	}

	return nullptr;
}

bool FScriptGeneratorManager::CanExportClass(IScriptGenerator *InGenerator) const
{
	return InGenerator && !m_Generators.Find(InGenerator->GetKey());
}

void FScriptGeneratorManager::ExportConfigClass()
{
	FString ConfigClassFileName = NS_LuaGenerator::ProjectPath/NS_LuaGenerator::ClassConfigFileRelativePath;

	TArray<FConfigClass> ConfigClasses;
	ParseConfigClass(ConfigClassFileName, ConfigClasses);

	for (const FConfigClass& ClassItem : ConfigClasses)
	{
		FString ConfigClass;
		
		DebugLog(TEXT("ClassItem.ParentName:%s"), *ClassItem.ParentName);

		for (const FConfigFunction& FunctionItem : ClassItem.Functions)
		{
			ConfigClass = FString::Printf(TEXT("%s %s::%s"), *FunctionItem.RetType, *ClassItem.Name, *FunctionItem.Name);
			if (FunctionItem.bStatic)
			{
				ConfigClass = "static " + ConfigClass;
			}

			FString Params;
			bool isNeedComma = false ;
			for (const FString& FunctionParamItem : FunctionItem.ParamTypes)
			{
				if (isNeedComma)
				{
					Params += ",";
				}
				else
				{
					isNeedComma = true;
				}

				Params += FunctionParamItem;
			}

			ConfigClass = FString::Printf(TEXT("%s(%s)"), *ConfigClass, *Params);
			DebugLog(TEXT("%s"), *ConfigClass);
		}
	}
}

void FScriptGeneratorManager::ParseConfigClass(const FString &FileName, TArray<FConfigClass> &OutConfigClasses)
{
	FString JsonStr;
	TArray<TSharedPtr<FJsonValue>> JsonClasses;
	if (!FFileHelper::LoadFileToString(JsonStr, *FileName))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("LoadFileToString from file %s found an error, please check again!"), *FileName);
		return;
	}
	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
	if (!FJsonSerializer::Deserialize(jsonReader, JsonClasses))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("LoadFileToJsonData from file %s found an error, please check again!"), *FileName);
		return ;
	}

	for (const TSharedPtr<FJsonValue> &pClassInfoItem : JsonClasses)
	{
		FConfigClass ConfigClass;
		const TSharedPtr<FJsonObject> pJsonClass = pClassInfoItem->AsObject();
		
		// parse class name
		if (!pJsonClass->TryGetStringField("ClassName", ConfigClass.Name))
		{
			UE_LOG(LogLuaGenerator, Error, TEXT("try get class name error!"));
			return;
		}

		// parse class parent name
		if (!pJsonClass->TryGetStringField("ParentName", ConfigClass.ParentName))
		{
			UE_LOG(LogLuaGenerator, Error, TEXT("try get class ParentName error!"));
			return;
		}

		// parse class functions 
		const TArray<TSharedPtr<FJsonValue>> *JsonFunctions;
		if (!pJsonClass->TryGetArrayField("Functions", JsonFunctions))
		{
			UE_LOG(LogLuaGenerator, Error, TEXT("try get class functions error!"));
			return;
		}
		else
		{
			for (const TSharedPtr<FJsonValue> &pFunctionInfoItem : *JsonFunctions)
			{
				FConfigFunction functionInfo;
				const TSharedPtr<FJsonObject> pJsonFunction = pFunctionInfoItem->AsObject();
				
				// parse function name
				if (!pJsonFunction->TryGetStringField("FuncName", functionInfo.Name))
				{
					UE_LOG(LogLuaGenerator, Error, TEXT("try get Function name error!"));
					return;
				}

				// parse function is static 
				if (!pJsonFunction->TryGetBoolField("IsStatic", functionInfo.bStatic))
				{
					UE_LOG(LogLuaGenerator, Error, TEXT("try get function IsStatic error!"));
					return;
				}

				// parse function RetType 
				if (!pJsonFunction->TryGetStringField("RetType", functionInfo.RetType))
				{
					UE_LOG(LogLuaGenerator, Error, TEXT("try get function RetType error!"));
					return;
				}

				// parse function param types
				const TArray<TSharedPtr<FJsonValue>> *JsonFuncParamTypes;
				if (!pJsonFunction->TryGetArrayField("ParamTypes", JsonFuncParamTypes))
				{
					UE_LOG(LogLuaGenerator, Error, TEXT("try get class function ParamTypes error!"));
					return;
				}
				else
				{
					for (const TSharedPtr<FJsonValue> &pFuncParamTypeInfo : *JsonFuncParamTypes)
					{
						// parse function param
						FString ParamType;
						if (pFuncParamTypeInfo->TryGetString(ParamType))
						{
							functionInfo.ParamTypes.Add(ParamType);
						}
						else
						{
							UE_LOG(LogLuaGenerator, Error, TEXT("try get class function ParamType item error!"));
							return;
						}
					}
				}

				ConfigClass.Functions.Add(functionInfo);
			}
		}

		OutConfigClasses.Add(ConfigClass);
	}
}
