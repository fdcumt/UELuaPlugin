#pragma once

class FLuaConfigManager
{
public:
	void Init();

public:
	FString ProjectPath;
	FString GameModuleName;
	FString ClassScriptHeaderSuffix;
	FString LuaConfigFileRelativePath;

	TArray<FString> SupportStructs;
	TArray<FString> AdditionalIncludeHeaders;
};
