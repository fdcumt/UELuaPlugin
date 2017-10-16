#include "LuaConfigManager.h"
#include "GeneratorDefine.h"
#include "ProjectDescriptor.h"
#include "CoreUObject.h"
#include "Misc/FileHelper.h"
#include "UObjectIterator.h"
#include "Serialization/JsonSerializer.h"

void FLuaConfigManager::Init()
{
	ProjectPath.Empty();
	GameModuleName.Empty();

	ClassScriptHeaderSuffix = ".script.h";
	ClassConfigFileRelativeFolder = "Config";

	FString ProjectFilePath = FPaths::GetProjectFilePath();
	ProjectPath = FPaths::GetPath(ProjectFilePath);
	LuaConfigFileRelativePath = FString("Config/LuaConfig.ini");
	FString ConfigFilePath = ProjectPath / LuaConfigFileRelativePath;

	GConfig->GetArray(TEXT("BaseTypes"), TEXT("TypeName"), BaseTypes, ConfigFilePath);
	GConfig->GetArray(TEXT("SupportStructs"), TEXT("StructName"), SupportStructs, ConfigFilePath);
	GConfig->GetArray(TEXT("SupportModules"), TEXT("ModuleName"), SupportedModules, ConfigFilePath);
	GConfig->GetArray(TEXT("NotSupportClass"), TEXT("NotSupportClassKey"), NotSuportClasses, ConfigFilePath);
	GConfig->GetArray(TEXT("ConfigClassFiles"), TEXT("ConfigClassFileName"), ClassConfigFileNames, ConfigFilePath);
	GConfig->GetArray(TEXT("AdditionalIncludeHeaders"), TEXT("IncludeHeader"), AdditionalIncludeHeaders, ConfigFilePath);

	if (!FPaths::IsProjectFilePathSet())
	{
		DebugLog(TEXT("Project file path not set!"));
	}

	FConfigFile* File = GConfig->Find(ProjectPath/LuaConfigFileRelativePath, false);
	if (File == nullptr)
	{
		DebugLog(TEXT("not found config file %s!"), *(ProjectPath / LuaConfigFileRelativePath));
	}

	FText OutError;
	FProjectDescriptor ProjectDescriptor;

	// FPaths::GetProjectFilePath() return the  path of project's .uproject file
	ProjectDescriptor.Load(ProjectFilePath, OutError);

	// init the GameModuleName by the first module name in the file of ProjectName.uproject 
	ProjectDescriptor.Modules[0].Name.ToString(GameModuleName);

	InitCorrectVariables();

}

void FLuaConfigManager::InitCorrectVariables()
{
	FString JsonStr;
	TArray<TSharedPtr<FJsonValue>> JsonClasses;
	FString ConfigFileName = ProjectPath / FString("Config/CorrectVariableType.json");
	if (!FFileHelper::LoadFileToString(JsonStr, *ConfigFileName))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("FLuaConfigManager::InitCorrectVariables loadFile %s error!"), *ConfigFileName);
		return;
	}

	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
	if (!FJsonSerializer::Deserialize(jsonReader, JsonClasses))
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("FLuaConfigManager::InitCorrectVariables from file %s found an error, please check again!"), *ConfigFileName);
		return;
	}

	for (const TSharedPtr<FJsonValue> &pClassInfoItem : JsonClasses)
	{
		FString className;
		TArray<FCorrectVariableInfo> correctVariableInfos;
		TSharedPtr<FJsonObject> pJsonObj = pClassInfoItem->AsObject();
		if (!pJsonObj->TryGetStringField("ClassName", className))
		{
			UE_LOG(LogLuaGenerator, Error, TEXT("FLuaConfigManager::InitCorrectVariables try get className error!"));
			return;
		}

		const TArray<TSharedPtr<FJsonValue>> *JsonCorrects;
		if (pJsonObj->TryGetArrayField("CorrectList", JsonCorrects))
		{
			FCorrectVariableInfo correctVariableInfo;
			for (const TSharedPtr<FJsonValue> &CorrectItem : *JsonCorrects)
			{
				TSharedPtr<FJsonObject> pJsonCorrectItem = CorrectItem->AsObject();
				if (!pJsonCorrectItem->TryGetStringField("VariableType", correctVariableInfo.VariableType))
				{
					UE_LOG(LogLuaGenerator, Error, TEXT("FLuaConfigManager::InitCorrectVariables try get VariableType error!"));
					return;
				}
				if (!pJsonCorrectItem->TryGetStringField("VariableName", correctVariableInfo.VariableName))
				{
					UE_LOG(LogLuaGenerator, Error, TEXT("FLuaConfigManager::InitCorrectVariables try get VariableName error!"));
					return;
				}
				if (!pJsonCorrectItem->TryGetStringField("PureType", correctVariableInfo.PureType))
				{
					UE_LOG(LogLuaGenerator, Error, TEXT("FLuaConfigManager::InitCorrectVariables try get PureType error!"));
					return;
				}
				if (!pJsonCorrectItem->TryGetStringField("DeclareType", correctVariableInfo.DeclareType))
				{
					UE_LOG(LogLuaGenerator, Error, TEXT("FLuaConfigManager::InitCorrectVariables try get DeclareType error!"));
					return;
				}
			}
			correctVariableInfos.Add(correctVariableInfo);
		}
		else
		{
			UE_LOG(LogLuaGenerator, Error, TEXT("FLuaConfigManager::InitCorrectVariables try get CorrectList error!"));
		}

		if (!CorrectVariableTypes.Contains(className))
		{
			CorrectVariableTypes.Add(className, correctVariableInfos);
		}
		else
		{
			UE_LOG(LogLuaGenerator, Error, TEXT("FLuaConfigManager::InitCorrectVariables there are same class in config file:%s!"), *ConfigFileName);
		}

	}
}
