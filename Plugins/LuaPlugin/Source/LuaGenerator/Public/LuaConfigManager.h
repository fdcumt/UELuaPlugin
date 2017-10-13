#pragma once

class FLuaConfigManager
{
public:
	void Init();

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
};
