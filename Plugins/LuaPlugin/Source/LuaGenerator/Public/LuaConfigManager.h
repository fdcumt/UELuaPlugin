#pragma once

class FLuaConfigManager
{
public:
	void Init();

public:
	FString ProjectPath;
	TArray<FString> SupportStructs;
	TArray<FString> AdditionalIncludeHeaders;
};
