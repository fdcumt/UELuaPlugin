#include "ScriptGeneratorManager.h"
#include "UnrealType.h"
#include "ClassGenerator.h"
#include "GeneratorDefine.h"
#include "Misc/FileHelper.h"
#include "UObjectIterator.h"
#include "ConfigClassGenerator.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "ConfigClassDefine.h"

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

	InitConfig();
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
	for (const FString&ConfigClassFileName : NS_LuaGenerator::ClassConfigFileNames)
	{
		TArray<FConfigClass> ConfigClasses;
		ParseConfigClass(NS_LuaGenerator::ProjectPath/NS_LuaGenerator::ClassConfigFileRelativeFolder/ConfigClassFileName, ConfigClasses);

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

void FScriptGeneratorManager::InitConfig()
{
	FString ConfigFilePath = NS_LuaGenerator::ProjectPath / NS_LuaGenerator::LuaConfigFileRelativePath;

	// init ClassConfigFileNames
	GConfig->GetArray(NS_LuaGenerator::ConfigClassFilesSection, NS_LuaGenerator::ConfigClassFileKey, NS_LuaGenerator::ClassConfigFileNames, ConfigFilePath);

	// init baseTypes
	GConfig->GetArray(NS_LuaGenerator::BaseTypeSection, NS_LuaGenerator::BaseTypeKey, NS_LuaGenerator::BaseTypes, ConfigFilePath);
}

void FScriptGeneratorManager::FinishExportPost()
{
	GenerateAndSaveAllHeaderFile();
	GererateLoadAllDefineFile();
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

	LoadAllDefineFile += EndLinePrintf(TEXT("#pragma once"));
	LoadAllDefineFile += EndLinePrintf(TEXT("#ifndef Def_LoadAll"));
	LoadAllDefineFile += EndLinePrintf(TEXT("#define Def_LoadAll(InLuaState) \\"));

	for (auto &MapItem : m_Generators)
	{
		IScriptGenerator *pGenerator = MapItem.Value;
		if (pGenerator->GetType() == NS_LuaGenerator::EConfigClass)
		{
			LoadAllDefineFile += EndLinePrintf(TEXT("\tFLuaUtil::RegisterClass(InLuaState, %s, \"%s\");\\"), *pGenerator->GetRegName(), *pGenerator->GetKey());
		}
	}

	LoadAllDefineFile += EndLinePrintf(TEXT(""));
	LoadAllDefineFile += EndLinePrintf(TEXT("#endif"));

	if (!FFileHelper::SaveStringToFile(LoadAllDefineFile, *(m_OutDir / LoadAllDefineFileName)))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("Failed to save LoadAllDefine.h:%s"), *(m_OutDir / LoadAllDefineFileName));
	}
}
