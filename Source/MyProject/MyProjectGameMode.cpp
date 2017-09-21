// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MyProjectGameMode.h"
#include "MyProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "LuaWrapperModule.h"
#include "LuaUtil.h"

DEFINE_LOG_CATEGORY(LogProject);

AMyProjectGameMode::AMyProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AMyProjectGameMode::StartPlay()
{
	FLuaWrapperModule& ModuleUI = FModuleManager::Get().LoadModuleChecked<FLuaWrapperModule>(FName("LuaWrapper"));
	ModuleUI.Init();

	int32 Ret1 = 0;
	int32 Ret2 = 0;
	bool Ret3 = false;

	FLuaUtil::CallR(
		Ret1,
		Ret2,
		Ret3,
		FLuaFuncName("Init"), 
		3);

	UE_LOG(LogProject, Log, TEXT("ret1:%d,ret2:%d,ret3:%d, "), Ret1, Ret2, (int32)Ret3);
}
