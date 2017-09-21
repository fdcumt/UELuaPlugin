// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyProjectGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogProject, Log, All);

UCLASS(minimalapi)
class AMyProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyProjectGameMode();

public:
	virtual void StartPlay() override;
};



