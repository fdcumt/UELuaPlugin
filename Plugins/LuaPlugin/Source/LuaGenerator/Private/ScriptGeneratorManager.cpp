#include "ScriptGeneratorManager.h"
#include "UnrealType.h"
#include "UClassGenerator.h"
#include "GeneratorDefine.h"
#include "Misc/FileHelper.h"
#include "UObjectIterator.h"
#include "ConfigClassGenerator.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "ConfigClassDefine.h"
#include "UStructGenerator.h"
#include "LuaConfigManager.h"

FScriptGeneratorManager::FScriptGeneratorManager()
{

}

FScriptGeneratorManager::~FScriptGeneratorManager()
{
	SafeDelete(g_LuaConfigManager);
}

void FScriptGeneratorManager::Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase)
{
	m_OutDir = g_LuaConfigManager->ProjectPath/FString("Plugins/LuaPlugin/Intermediate/Build/Win64/UE4Editor/Inc/LuaWrapper");
	m_RootLocalPath = RootLocalPath;
	m_RootBuildPath = RootBuildPath;
	m_IncludeBase = IncludeBase;

	InitConfig();
}

void FScriptGeneratorManager::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	IScriptGenerator *pGenerator = new FUClassGenerator(Class, m_OutDir, SourceHeaderFilename);
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

	InitClassParentManager();

	SaveToFiles();
	FinishExportPost();
}

bool FScriptGeneratorManager::ContainClassName(const FString &ClassName)
{
	return m_Generators.Contains(ClassName);
}

TArray<FString> FScriptGeneratorManager::GetParentNames(const FString &ClassName)
{
	return m_ClassParentManager.GetParentClassNames(ClassName);
}

IScriptGenerator* FScriptGeneratorManager::GetGenerator(const FString &ClassName)
{
	IScriptGenerator **ppGenerator = m_Generators.Find(ClassName);
	return ppGenerator == nullptr ? nullptr : *ppGenerator;
}

bool FScriptGeneratorManager::CanExportClass(IScriptGenerator *InGenerator) const
{
	return !m_Generators.Find(InGenerator->GetKey());
}

void FScriptGeneratorManager::ExportExtrasToMemory()
{
	ExportConfigClasses();
	ExportUStructs();
}

void FScriptGeneratorManager::AdjustBeforeSaveToFile()
{

}

void FScriptGeneratorManager::InitClassParentManager()
{
	TArray<IScriptGenerator*> ScriptGenerators;
	m_Generators.GenerateValueArray(ScriptGenerators);
	m_ClassParentManager.Init(ScriptGenerators);
}

void FScriptGeneratorManager::ExportConfigClasses()
{
	for (const FString&ConfigClassFileName : g_LuaConfigManager->ClassConfigFileNames)
	{
		TArray<FConfigClass> ConfigClasses;
		ParseConfigClass(g_LuaConfigManager->ProjectPath/g_LuaConfigManager->ClassConfigFileRelativeFolder/ConfigClassFileName, ConfigClasses);

		for (const FConfigClass& ClassItem : ConfigClasses)
		{
			ExportConfigClass(ClassItem);
		}
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
		OutConfigClasses.Add(FConfigClass(pClassInfoItem->AsObject()));
	}
}

void FScriptGeneratorManager::ExportUStructs()
{
	for (TObjectIterator<UScriptStruct> It; It; ++It)
	{
		ExportUStruct(*It);
	}
}

void FScriptGeneratorManager::ExportUStruct(UScriptStruct *pScriptStruct)
{
	IScriptGenerator *pGenerator = FUStructGenerator::CreateGenerator(pScriptStruct, m_OutDir);
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

void FScriptGeneratorManager::AddGeneratorToMap(IScriptGenerator *InGenerator)
{
	m_Generators.Add(InGenerator->GetKey(), InGenerator);
}

void FScriptGeneratorManager::SaveToFiles()
{
// 	SaveUClassesToFiles();
// 	SaveConfigClassesToFiles();

	for (auto &MapItem : m_Generators)
	{
		IScriptGenerator *pGenerator = MapItem.Value;
		pGenerator->SaveToFile();
	}
}

void FScriptGeneratorManager::SaveUClassesToFiles()
{
	for (auto &MapItem : m_Generators)
	{
		IScriptGenerator *pGenerator = MapItem.Value;
		if (pGenerator->GetType() == NS_LuaGenerator::EUClass)
		{
			pGenerator->SaveToFile();
		}
	}
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

void FScriptGeneratorManager::InitConfig()
{

}

void FScriptGeneratorManager::FinishExportPost()
{
	GenerateAndSaveAllHeaderFile();
	GererateLoadAllDefineFile();

	FString PropertyTypes;
	for (const FString &Item : m_PropertyType)
	{
		PropertyTypes += Item;
	}

	FFileHelper::SaveStringToFile(PropertyTypes, *(m_OutDir / FString("PropertyTypes.txt")));
	FFileHelper::SaveStringToFile(m_LogContent, *(m_OutDir / FString("GeneratorLog.txt")));
}

void FScriptGeneratorManager::GenerateAndSaveAllHeaderFile()
{
	FString AllHeaderFileName("AllHeaders.h");
	TSet<FString> HeaderFileNames;
	FString AllHeaderFileContent;

	AllHeaderFileContent += EndLinePrintf(TEXT("#pragma once"));

	for (auto &MapItem : m_Generators)
	{
		IScriptGenerator *pGenerator = MapItem.Value;
		HeaderFileNames.Add(*pGenerator->GetFileName());
	}

	for (const FString &IncludeHeader : g_LuaConfigManager->AdditionalIncludeHeaders)
	{
		AllHeaderFileContent += EndLinePrintf(TEXT("#include \"%s\""), *IncludeHeader);
	}

	for (const FString &FileName : HeaderFileNames)
	{
		AllHeaderFileContent += EndLinePrintf(TEXT("#include \"%s\""), *FileName);
	}

	if (!FFileHelper::SaveStringToFile(AllHeaderFileContent, *(m_OutDir/AllHeaderFileName)))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("Failed to save AllHeaders.h:%s"), *(m_OutDir / AllHeaderFileName));
	}
}

void FScriptGeneratorManager::GererateLoadAllDefineFile()
{
	FString LoadAllDefineFileName("LoadAllDefine.h");
	TMap<FString, FString> RegLibsMap;
	FString LoadAllDefineFile;

	LoadAllDefineFile += EndLinePrintf(TEXT("#pragma once"));
	LoadAllDefineFile += EndLinePrintf(TEXT("#ifndef Def_LoadAll"));
	LoadAllDefineFile += EndLinePrintf(TEXT("#define Def_LoadAll(InLuaState) \\"));

	for (auto &MapItem : m_Generators)
	{
		IScriptGenerator *pGenerator = MapItem.Value;
		RegLibsMap.Add(pGenerator->GetRegName(), pGenerator->GetKey());
	}

	for (auto &RegLibItem : RegLibsMap)
	{
		FString RegLibName = RegLibItem.Key;
		FString LuaClassName = RegLibItem.Value;
		LoadAllDefineFile += EndLinePrintf(TEXT("\tFLuaUtil::RegisterClass(InLuaState, %s, \"%s\");\\"), *RegLibName, *LuaClassName);
	}

	LoadAllDefineFile += EndLinePrintf(TEXT(""));
	LoadAllDefineFile += EndLinePrintf(TEXT("#endif"));

	if (!FFileHelper::SaveStringToFile(LoadAllDefineFile, *(m_OutDir / LoadAllDefineFileName)))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("Failed to save LoadAllDefine.h:%s"), *(m_OutDir / LoadAllDefineFileName));
	}
}
