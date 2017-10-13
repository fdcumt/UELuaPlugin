#pragma once

class FLuaConfigManager
{
public:
	void Init();

public:
	TArray<FString> SupportStructs;
	TArray<FString> AdditionalIncludeHeaders;
};
