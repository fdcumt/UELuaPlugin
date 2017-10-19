#pragma once

struct FCorrectVariable
{
	FString VariableType;
	FString VariableName;
	FString PureType;
	FString DeclareType;
};

struct FExportConfig
{
	TArray<FString> NotExportFunctions;
	TArray<FCorrectVariable> correctVariables;
};

class FLuaConfigManager
{
public:
	void Init();

private:
	void InitExportConfig();

public:
	TArray<FString> SupportedModules;
	TArray<FString> NotSuportClasses;
	TArray<FString> SupportStructs;
	TArray<FString> BaseTypes;
	TArray<FString> ClassConfigFileNames;
	TArray<FString> AdditionalIncludeHeaders;

	FString ProjectPath;
	FString GameModuleName;
	FString ClassScriptHeaderSuffix;
	FString LuaConfigFileRelativePath;
	FString ClassConfigFileRelativeFolder;

	// config 
	TArray<FString> InvalidSetTypes;
	TMap<FString, FExportConfig> ExportConfig;
};
