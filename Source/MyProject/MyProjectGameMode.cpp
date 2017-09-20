// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MyProjectGameMode.h"
#include "MyProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "LuaWrapperModule.h"
#include "LuaUtil.h"

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

	FLuaUtil::Call("Init");
}
