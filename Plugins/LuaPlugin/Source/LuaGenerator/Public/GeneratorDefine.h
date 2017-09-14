#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogLuaGenerator, Log, All);

#define StringPrintfWithEndLine(FormatString, ...) FString::Printf(TEXT("%s\r\n"), *FString::Printf(FormatString, ##__VA_ARGS__ ))

namespace NS_LuaGenerator
{
	enum E_GeneratorType
	{
		EClass = 0,
	};

	extern FString ProjectPath;
	extern FString GameModuleName;
	extern FString ClassScriptHeaderSuffix;

	extern const TCHAR* SupportModuleKey;
	extern const TCHAR* SupportModuleSection;

	extern const TCHAR* NotSupportClassSection;
	extern const TCHAR* NotSupportClassKey;

	extern const FString LuaConfigFileRelativePath;
}

