#pragma once

struct FCorrectVariableInfo
{
	FString VariableType;
	FString VariableName;
	FString PureType;
	FString DeclareType;
};

class FLuaConfigManager
{
public:
	void Init();

private:
	void InitCorrectVariables();

public:
	TArray<FString> SupportedModules;
	TArray<FString> NotSuportClasses;
	TArray<FString> BaseTypes;
	TArray<FString> ClassConfigFileNames;

	FString ProjectPath;
	FString GameModuleName;
	FString ClassScriptHeaderSuffix;
	FString LuaConfigFileRelativePath;
	FString ClassConfigFileRelativeFolder;

	TArray<FString> SupportStructs;
	TArray<FString> AdditionalIncludeHeaders;
	TMap<FString, TArray<FCorrectVariableInfo> > CorrectVariableTypes;
};
