// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyManager.h"
#include "BimBall/Entity/Enemy/BaseEnemy.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "BimBall/GameManagement/BimBallGameMode.h"
#include "BimBall/Props/Ball/Ball.h"
#include "EngineUtils.h"

/**================================ PLAYER/BALL ================================*/

void UEnemyManager::Set(ABimBallGameMode* gameMode_, ABimBallCharacter* playerChar_)
{
	/* Set Ref */
	if (gameMode_)
		gameMode = gameMode_;
	if (playerChar_)
	{
		playerChar = playerChar_;

		if (playerChar->GetBall())
		{
			ball = playerChar->GetBall();
		}
	}
}

/**================================ ENEMIES_MANAGE ================================*/

void UEnemyManager::ResetAtkingIndex()
{
	for (int32 index = 0; index < atkingEnemies.Num(); index++)
	{
		ABaseEnemy* enemy = atkingEnemies[index];

		if (enemy)
		{
			enemy->atkingIndex = index;
		}
	}
}

void UEnemyManager::ResetEngageIndex()
{
	for (int32 index = 0; index < engagedEnemies.Num(); index++)
	{
		ABaseEnemy* enemy = engagedEnemies[index];
		
		if (enemy)
		{	
			enemy->engageIndex = index;
		}
	}
}

void UEnemyManager::ResetIndex()
{
	for (int32 index = 0; index < enemies.Num(); index++)
	{
		ABaseEnemy* enemy = enemies[index];
		
		if (enemy)
		{
			if (enemy->managerIndex == ballCarryingEnemy)
				ballCarryingEnemy = -2;
		
			enemy->managerIndex = index;
		}
	}
}

bool UEnemyManager::AtkQuerry(ABaseEnemy* newEnemy_)
{
	if (!newEnemy_)
		return false;

	/* enemy needs to be engaged to attack */
	if (atkingEnemies.Num() >= maxAtkingEnemiesNb || newEnemy_->engageIndex < 0)
	{
		return false;
	}
	/* makes it easier to remove later, and can be useful to get enemies current behavior */
	newEnemy_->atkingIndex = atkingEnemies.Add(newEnemy_);
	return true;
}

void UEnemyManager::AddEnemy(ABaseEnemy* newEnemy_)
{
	/* Guards */
	if (newEnemy_)
		newEnemy_->managerIndex = enemies.Add(newEnemy_);
}

void UEnemyManager::RemoveEnemy(ABaseEnemy* enemy_)
{
	/* Guards */
	if (enemy_)
	{
		RemoveEngageEnemy(enemy_);

		if (enemy_->managerIndex == ballCarryingEnemy)
			ballCarryingEnemy = -2;

		enemies.RemoveAt(enemy_->managerIndex);
		ResetIndex();

		enemy_->engageIndex = -1;
		enemy_->managerIndex = -1;
	}
}

void UEnemyManager::RemoveEngageEnemy(ABaseEnemy* enemy_)
{
	/* Guards */
	if (enemy_)
	{
		if (enemy_->engageIndex >= 0)
		{	
			RemoveAtkingEnemies(enemy_);

			engagedEnemies.RemoveAt(enemy_->engageIndex);
			ResetEngageIndex();
		}
		enemy_->engageIndex = -1;
	}
}

void UEnemyManager::RemoveAtkingEnemies(ABaseEnemy* enemy_)
{
	/* Guards */
	if (enemy_)
	{
		if (enemy_->atkingIndex >= 0)
		{
			atkingEnemies.RemoveAt(enemy_->atkingIndex);
			ResetAtkingIndex();
		}

		enemy_->atkingIndex = -1;
	}
}

void UEnemyManager::RemoveFarestEngageEnemy()
{
	/* Guards */
	if (!playerChar)
		return;

	float compareDist = 0.0f;
	int32 removeEnemy = 0;
	FVector playerPos = playerChar->GetActorLocation();

	for (int32 index = 0; index < engagedEnemies.Num(); index++)
	{
		ABaseEnemy* enemy = engagedEnemies[index];

		if (enemy)
		{
			float indexDist = (enemy->GetActorLocation() - playerPos).SizeSquared();
			if (indexDist >= compareDist)
			{
				removeEnemy = index;
				compareDist = indexDist;
			}
		}
	}

	engagedEnemies.RemoveAt(removeEnemy);
	ResetEngageIndex();
}

bool UEnemyManager::EngageQuerry(ABaseEnemy* enemy_)
{
	/* Guards */
	if (!enemy_ || engagedEnemies.Num() >= maxEngagedEnemiesNb)
		return false;

	enemy_->engageIndex =  engagedEnemies.Add(enemy_);

	return true;
}

void UEnemyManager::ForceEngage(ABaseEnemy* newEnemy_)
{
	/* Guards */
	if (!newEnemy_  || newEnemy_->engageIndex > 0)
		return;

	RemoveFarestEngageEnemy();

	newEnemy_->engageIndex = engagedEnemies.Add(newEnemy_);
}

FVector UEnemyManager::PreventMoveAnnoyance(ABaseEnemy* movingEnemy, FVector wantedLocation)
{
	for (ABaseEnemy* enemy : enemies)
	{
		if (enemy && enemy != movingEnemy)
		{
			FVector wantedToEnemy = (wantedLocation - enemy->GetActorLocation());
			if (wantedToEnemy.SizeSquared() <= (distBetweenEnemies * distBetweenEnemies))
			{
				/* enemy usually look where they want to go so you can side step by taking the right vector */
				FVector sideStep = FVector::DotProduct(enemy->GetActorRightVector(), wantedToEnemy) * enemy->GetActorRightVector();
				if (sideStep.Normalize())
				{
					wantedLocation += sideStep * distBetweenEnemies;
				}
			}
		}
	}

	return wantedLocation;
}