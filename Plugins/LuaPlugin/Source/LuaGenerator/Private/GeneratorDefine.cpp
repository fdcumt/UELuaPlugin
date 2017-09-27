#include "GeneratorDefine.h"


DEFINE_LOG_CATEGORY(LogLuaGenerator);

namespace NS_LuaGenerator
{
	FString ProjectPath;
	FString GameModuleName;
	FString ClassScriptHeaderSuffix(".script.h"); 

	const FString LuaConfigFileRelativePath("Config/LuaConfig.ini");

	const TCHAR* SupportModuleSection = TEXT("SupportModule");
	const TCHAR* SupportModuleKey = TEXT("SupportModuleKey");

	const TCHAR* NotSupportClassSection = TEXT("NotSupportClass");
	const TCHAR* NotSupportClassKey = TEXT("NotSupportClassKey");

	const TCHAR* BaseTypeSection = TEXT("BaseType");
	const TCHAR* BaseTypeKey = TEXT("BaseTypeKey");
	TArray<FString> BaseTypes;

	const TCHAR* ConfigClassFilesSection = TEXT("ConfigClassFiles");
	const TCHAR* ConfigClassFileKey = TEXT("ConfigClassFileName");
	const FString ClassConfigFileRelativeFolder("Config");
	TArray<FString> ClassConfigFileNames;

	bool StringFowwardContainSub(const FString &&SrcStr, const FString &&SubStr, int32 SrcIndex)
	{
		int32 SubIndex = 0;

		if (SubStr.IsEmpty())
		{
			return false;
		}

		while (true)
		{
			if (SubIndex == SubStr.Len())
			{
				return true;
			}

			if (SrcIndex>=SrcStr.Len())
			{
				return false;
			}

			if (SrcStr[SrcIndex]!=SubStr[SubIndex])
			{
				return false;
			}

			++SrcIndex;
			++SubIndex;
		}
	}

	bool StringBackContainSub(const FString &&SrcStr, const FString &&SubStr, int32 SrcTailIndex)
	{
		int32 SubTailIndex = SubStr.Len()-1;

		if (SubStr.IsEmpty())
		{
			return false;
		}

		while (true)
		{
			if (SubTailIndex == -1)
			{
				return true;
			}

			if (SrcTailIndex ==-1)
			{
				return false;
			}

			if (SrcStr[SrcTailIndex] != SubStr[SubTailIndex])
			{
				return false;
			}

			--SrcTailIndex;
			--SubTailIndex;
		}
	}

}
