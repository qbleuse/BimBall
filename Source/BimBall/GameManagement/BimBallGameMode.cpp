// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BimBallGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "BimBall/Entity/Enemy/EnemyManager.h"
#include "BimBall/Entity/Enemy/BaseEnemy.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "BimBall/Level/LevelSections/Yard.h"
#include "EngineUtils.h"

/**================================ ABIMBALLGAMEMODE ================================*/

ABimBallGameMode::ABimBallGameMode()
{

}

void ABimBallGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName,Options,ErrorMessage);

	if (*subEnemyManager)
		enemyManager = NewObject<UEnemyManager>(this,*subEnemyManager);
	else
		enemyManager = NewObject<UEnemyManager>();
}

void ABimBallGameMode::BeginPlay()
{
	TActorIterator<AYard> yardIt = TActorIterator<AYard>(GetWorld());

	for (; yardIt; ++yardIt)
	{
		yards.Add(*yardIt);
	}
}

void ABimBallGameMode::SetPlayerChar(ABimBallCharacter* playerChar_)
{
	if (playerChar_)
	{
		enemyManager->Set(this,playerChar_);
	}
}

/**================================ ENEMY_MANAGER ================================*/

void ABimBallGameMode::Win_Implementation()
{
	bWin = true;
}

void ABimBallGameMode::Lose_Implementation()
{
	bLose = true;
}

void ABimBallGameMode::ResetWinLose_Implementation()
{
	bWin  = false;
	bLose = false;
}