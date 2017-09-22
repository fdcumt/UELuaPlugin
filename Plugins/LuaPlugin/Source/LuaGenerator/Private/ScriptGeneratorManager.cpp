#include "ScriptGeneratorManager.h"
#include "UnrealType.h"
#include "ClassGenerator.h"
#include "GeneratorDefine.h"
#include "Misc/FileHelper.h"
#include "UObjectIterator.h"
#include "ConfigClassGenerator.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"

FScriptGeneratorManager::FScriptGeneratorManager()
{

}

FScriptGeneratorManager::~FScriptGeneratorManager()
{

}

void FScriptGeneratorManager::Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase)
{
	m_OutDir = NS_LuaGenerator::ProjectPath/FString("Plugins/LuaPlugin/Intermediate/Build/Win64/UE4Editor/Inc/LuaWrapper");
	m_RootLocalPath = RootLocalPath;
	m_RootBuildPath = RootBuildPath;
	m_IncludeBase = IncludeBase;

	FPaths::GamePluginsDir();

	DebugLog(TEXT("m_RootLocalPath: %s"), *m_RootLocalPath);
	DebugLog(TEXT("m_RootBuildPath: %s"), *m_RootBuildPath);
	DebugLog(TEXT("m_OutputDirectory: %s"), *m_OutDir);
}

void FScriptGeneratorManager::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	IScriptGenerator *pGenerator = FClassGenerator::CreateGenerator(Class, m_OutDir);

	if (pGenerator && CanExportClass(pGenerator) && pGenerator->CanExport())
	{
		pGenerator->ExportToMemory();
		AddGeneratorToMap(pGenerator);
	}
	else
	{
		SafeDelete(pGenerator);
	}
}

void FScriptGeneratorManager::FinishExport()
{
	ExportExtrasToMemory();
	AdjustBeforeSaveToFile();
	SaveToFiles();
	FinishExportPost();
}

bool FScriptGeneratorManager::CanExportClass(IScriptGenerator *InGenerator) const
{
	return !m_Generators.Find(InGenerator->GetKey());
}

void FScriptGeneratorManager::ExportExtrasToMemory()
{
	ExportConfigClasses();
}

void FScriptGeneratorManager::AdjustBeforeSaveToFile()
{

}

void FScriptGeneratorManager::ExportConfigClasses()
{
	TArray<FConfigClass> ConfigClasses;
	ParseConfigClass(NS_LuaGenerator::ProjectPath/NS_LuaGenerator::ClassConfigFileRelativePath, ConfigClasses);

	for (const FConfigClass& ClassItem : ConfigClasses)
	{
		ExportConfigClass(ClassItem);
	}
}

void FScriptGeneratorManager::ExportConfigClass(const FConfigClass& ClassItem)
{
	IScriptGenerator *pGenerator = FConfigClassGenerator::CreateGenerator(ClassItem, m_OutDir);
	if (pGenerator && CanExportClass(pGenerator) && pGenerator->CanExport() )
	{
		pGenerator->ExportToMemory();
		AddGeneratorToMap(pGenerator);
	}
	else
	{
		SafeDelete(pGenerator);
	}
}

void FScriptGeneratorManager::ParseConfigClass(const FString &&FileName, TArray<FConfigClass> &OutConfigClasses)
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

		// parse class header files 
		const TArray<TSharedPtr<FJsonValue>> *JsonHeaderFiles;
		if (!pJsonClass->TryGetArrayField("HeaderFiles", JsonHeaderFiles))
		{
			UE_LOG(LogLuaGenerator, Error, TEXT("try get class HeaderFiles error!"));
			return;
		}
		else
		{
			for (const TSharedPtr<FJsonValue> &pJsonHeaderFile : *JsonHeaderFiles)
			{
				// parse header file
				FString HeaderFile;
				if (pJsonHeaderFile->TryGetString(HeaderFile))
				{
					ConfigClass.IncludeHeaders.Add(HeaderFile);
				}
				else
				{
					UE_LOG(LogLuaGenerator, Error, TEXT("try get class header file item error!"));
					return;
				}
			}
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

void FScriptGeneratorManager::AddGeneratorToMap(IScriptGenerator *InGenerator)
{
	m_Generators.Add(InGenerator->GetKey(), InGenerator);
}

void FScriptGeneratorManager::SaveToFiles()
{
	SaveConfigClassesToFiles();
}

void FScriptGeneratorManager::SaveConfigClassesToFiles()
{
	for (auto &MapItem : m_Generators)
	{
		IScriptGenerator *pGenerator = MapItem.Value;
		if (pGenerator->GetType()==NS_LuaGenerator::EConfigClass)
		{
			pGenerator->SaveToFile();
		}
	}
}

void FScriptGeneratorManager::FinishExportPost()
{
	DebugLog(TEXT("FinishExportPost"));
	GenerateAndSaveAllHeaderFile();
	GererateLoadAllDefineFile();
	DebugLog(TEXT("FinishExportPost end"));
}

void FScriptGeneratorManager::GenerateAndSaveAllHeaderFile()
{
	FString AllHeaderFileName("AllHeaders.h");
	FString AllHeaderFile;

	AllHeaderFile += EndLinePrintf(TEXT("#pragma once"));

	for (auto &MapItem : m_Generators)
	{
		IScriptGenerator *pGenerator = MapItem.Value;
		if (pGenerator->GetType() == NS_LuaGenerator::EConfigClass)
		{
			AllHeaderFile += EndLinePrintf(TEXT("#include \"%s\""), *pGenerator->GetFileName());
		}
	}

	if (!FFileHelper::SaveStringToFile(AllHeaderFile, *(m_OutDir/AllHeaderFileName)))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("Failed to save AllHeaders.h:%s"), *(m_OutDir / AllHeaderFileName));
	}
}

void FScriptGeneratorManager::GererateLoadAllDefineFile()
{
	FString LoadAllDefineFileName("LoadAllDefine.h");
	FString LoadAllDefineFile;
	DebugLog(TEXT("FinishExportPost 1"));

	LoadAllDefineFile += EndLinePrintf(TEXT("#pragma once"));
	LoadAllDefineFile += EndLinePrintf(TEXT("#ifndef Def_LoadAll"));
	//LoadAllDefineFile += FString::Printf(TEXT("#define Def_LoadAll() \r\n\\"));
	LoadAllDefineFile += EndLinePrintf(TEXT("#define Def_LoadAll() \\"));



	for (auto &MapItem : m_Generators)
	{
		IScriptGenerator *pGenerator = MapItem.Value;
		if (pGenerator->GetType() == NS_LuaGenerator::EConfigClass)
		{
			DebugLog(TEXT("FinishExportPost 2"));

			LoadAllDefineFile += EndLinePrintf(TEXT("\tFLuaUtil::RegisterClass(%s, \"%s\");\\"), *pGenerator->GetRegName(), *pGenerator->GetKey());
			DebugLog(TEXT("FinishExportPost 3"));

		}
	}
	DebugLog(TEXT("FinishExportPost 4"));


	LoadAllDefineFile += EndLinePrintf(TEXT(""));
	DebugLog(TEXT("FinishExportPost 8"));

	LoadAllDefineFile += EndLinePrintf(TEXT("#endif"));

	DebugLog(TEXT("FinishExportPost 9"));

	if (!FFileHelper::SaveStringToFile(LoadAllDefineFile, *(m_OutDir / LoadAllDefineFileName)))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("Failed to save LoadAllDefine.h:%s"), *(m_OutDir / LoadAllDefineFileName));
	}
}
