// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "BimBall/Level/LevelSections/Yard.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "BimBall/GameManagement/BimBallGameMode.h"
#include "BimBall/Props/Ball/Ball.h"

/**================================ AENEMYCONTROLLER ================================*/

void AEnemyController::OnPossess(APawn* InPawn)
{
	/* Get Ref of Enemy */
	Super::OnPossess(InPawn);
	if (InPawn)
	{
		ctrldEnemy = Cast<ABaseEnemy>(InPawn);
	}

	/* Get Ref of Enemy Manager and register Enemy in Manager*/
	ABimBallGameMode* gameMode = Cast<ABimBallGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode)
		enemyManager = gameMode->GetEnemyManager();
	if (enemyManager)
		enemyManager->AddEnemy(ctrldEnemy);
}

void AEnemyController::OnUnPossess()
{
	/* UnRegister enemy of Manager when Unpossesed */
	Super::OnUnPossess();
	if (ctrldEnemy && ctrldEnemy->managerIndex >= 0)
	{
		enemyManager->RemoveEnemy(ctrldEnemy);
	}
}

/**================================ MOVEMENT ================================*/

bool AEnemyController::GetTo(FVector point, float acceptanceRadius)
{
	/* Guards */
	if (ctrldEnemy && enemyManager)
	{
		/* move point if an enemy is already there */
		MoveToLocation(enemyManager->PreventMoveAnnoyance(ctrldEnemy,point));

		bool moveStatus = FVector::PointsAreNear(point, ctrldEnemy->GetActorLocation(), acceptanceRadius);

		if (moveStatus)
		{
			StopMovement();
		}

		return moveStatus;
	}

	return true;
}

bool AEnemyController::GetToPlayer()
{
	/* Guards */
	if (enemyManager && enemyManager->GetPlayerChar())
	{
		FVector playerPos = enemyManager->GetPlayerChar()->GetActorLocation();

		return GetTo(playerPos,toPlayerAcceptanceRadius);
	}

	return true;
}

bool AEnemyController::GetToBall()
{
	/* Guards */
	if (ctrldEnemy && enemyManager && enemyManager->GetBall())
	{
		FVector ballPos = enemyManager->GetBall()->GetActorLocation();

		return GetTo(ballPos,0.0f);
	}

	return true;
}

/**================================ DECORATOR ================================*/

bool AEnemyController::AtDist(FVector targetLoc, float maxDist)const
{
	/* Guards */
	if (!ctrldEnemy)
		return false;

	float dist = (ctrldEnemy->GetActorLocation() - targetLoc).SizeSquared();

	return (dist <= maxDist);
}

bool AEnemyController::AtChaseDist()const
{
	/* Guards */
	if (!ctrldEnemy || !enemyManager || !enemyManager->GetPlayerChar())
		return false;

	FVector playerPos = enemyManager->GetPlayerChar()->GetActorLocation();

	bool answer = (AtDist(playerPos, ctrldEnemy->GetSqrdChaseDist()));

	/* automatically unengage enemy if too far from player */
	if (!answer && ctrldEnemy->engageIndex > 0)
		UnEngageEnemy();

	return answer;
}

bool AEnemyController::AtBallPickUpDist()const
{
	/* Guards */
	if (!enemyManager || !enemyManager->GetBall())
		return false;

	FVector ballPos = enemyManager->GetBall()->GetActorLocation();

	return AtDist(ballPos, ctrldEnemy->GetSqrdBallPickUpDist());
}

bool AEnemyController::CanAttack()const
{
	/* Guards */
	if (enemyManager && ctrldEnemy)
	{
		/* if already attacking (ususally used to make an ennemy attack again after a stagger)*/
		if (ctrldEnemy->atkingIndex >= 0)
		{
			return true;
		}
		if (AtChaseDist())
		{
			if (IsEngaged() && ctrldEnemy->GetCanAttack())
			{
				/* ask for permission */
				bool answer = enemyManager->AtkQuerry(ctrldEnemy);

				if (answer)
					ctrldEnemy->SetAtkingSpd();

				return answer;
			}/* try engage ennemy if only engage is needed to attack */
			else if (!IsEngaged())
				enemyManager->EngageQuerry(ctrldEnemy);
		}
	}

	return false;
}

bool AEnemyController::IsBallDropped()const
{
	/* Guards */
	if (enemyManager && enemyManager->GetBall())
	{
		return (enemyManager->GetBall()->GetState() == EBallState::IS_DROPPED);
	}

	return false;
}


/**================================ TASK ================================*/

void AEnemyController::MoveToSurround()
{
	/* Try engage ennemy, as it can */
	if (ctrldEnemy)
	{
		if (ctrldEnemy->engageIndex < 0 && AtChaseDist())
			enemyManager->EngageQuerry(ctrldEnemy);
		else if (!AtChaseDist())/* UnEngage enemy if condition are not completed anymore */
			UnEngageEnemy();
	}

	FVector playerPos		= enemyManager->GetPlayerChar()->GetActorLocation();
	FVector enemyToPlayer	= (ctrldEnemy->GetActorLocation() - playerPos);
	enemyToPlayer.Z			= 0.0f;

	/* try to surround if engaged */
	if (ctrldEnemy->engageIndex >= 0)
	{
		if (enemyToPlayer.SizeSquared() >= ctrldEnemy->GetSqrdSurroundDist())
		{
			enemyToPlayer.Normalize();
			GetTo(playerPos + enemyToPlayer * ctrldEnemy->GetSurroundDist(),moveToAcceptanceRadius);
		}
	}

	/* if player to close draw back  */
	if (enemyToPlayer.SizeSquared() <= ctrldEnemy->GetSqrdRunAwayDist())
	{
		enemyToPlayer.Normalize();
		GetTo(playerPos + enemyToPlayer * ctrldEnemy->GetRunAwayDist(), moveToAcceptanceRadius);
	}

	/* look at player */
	ctrldEnemy->SetActorRotation(FRotator(0.0f,(-enemyToPlayer).Rotation().Yaw,0.0f));
}

void AEnemyController::MoveToBlock()
{
	/* Guards */
	if (!ctrldEnemy || !enemyManager)
		return;

	FVector playerPos	= enemyManager->GetPlayerChar()->GetActorLocation();
	float distToPlayer	= FVector::DotProduct(ctrldEnemy->GetActorForwardVector(), playerPos);

	/* if player got around the block, a new blockLoc is computed, or if not blockLoc has been computed*/
	if (distToPlayer <= 0.0f || ctrldEnemy->blockLoc == FVector::ZeroVector)
	{
		
		FVector ballCarryingEnemyPos	= enemyManager->GetBallCaryingEnemy()->GetActorLocation();
		FVector playerToBall			= (ballCarryingEnemyPos - playerPos) * ctrldEnemy->GetBlockRatio();

		ctrldEnemy->SetActorRotation(FRotator(0.0f, (-playerToBall).Rotation().Yaw, 0.0f));

		ctrldEnemy->blockLoc			= playerPos + playerToBall;
	}


	ctrldEnemy->SetBlockSpd();

	GetTo(ctrldEnemy->blockLoc, moveToAcceptanceRadius);
}

void AEnemyController::MoveToGate()
{
	/* Guards */
	if (!ctrldEnemy || !enemyManager || !enemyManager->GetGameMode())
		return;

	ctrldEnemy->SetBallHoldingSpd();

	GetTo(ctrldEnemy->touchDownLoc,0.0f);
}

bool AEnemyController::TryAttack()
{
	/* Guards */
	if (!enemyManager || !ctrldEnemy)
		return true;

	if (AtChaseDist())
	{
		/* Call the attack in enemy */
		if (ctrldEnemy->TryAttack(enemyManager->GetPlayerChar()))
		{
			RmAtkingEnemy();
			return true;
		}
		/* when enemy is attacking and can attack is set to false,
		 * it pause its attack, that doesn't disable it */
		if (!ctrldEnemy->GetCanAttack())
		{
			StopMovement();
		}

		return false;
	}

	/* disable everything if too far */
	ctrldEnemy->DisableAttack();
	UnEngageEnemy();
	StopMovement();

	return true;
}

bool AEnemyController::TryPickUpBall()
{
	/* Guards */
	if (ctrldEnemy && enemyManager && IsBallDropped())
	{
		/* Call the pick up in enemy */
		if (ctrldEnemy->TryPickUp(enemyManager->GetBall()))
		{
			/* register in enemy Manager */
			enemyManager->ballCarryingEnemy = ctrldEnemy->managerIndex;
			enemyManager->ballBeingCarried  = true;
			UnEngageEnemy();
			return true;
		}

		return false;
	}

	return true;
}

FVector AEnemyController::GetClosestYard(FVector enemyLoc)
{
	FVector returnLoc = FVector::ZeroVector;
	float distFromLoc = MAX_flt;

	/* Guards */
	if (enemyManager && enemyManager->GetGameMode())
	{
		TArray<AYard*> yards = enemyManager->GetGameMode()->yards;

		for (AYard* yard : yards)
		{
			float yardDist = (enemyLoc - yard->GetActorLocation()).SizeSquared();
			if (yardDist <= distFromLoc)
			{
				returnLoc	= yard->GetActorLocation();
				distFromLoc = yardDist;
			}
		}
	}

	return returnLoc;
}