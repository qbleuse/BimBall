// Fill out your copyright notice in the Description page of Project Settings.


#include "TKEnemy.h"
#include "AI/EnemyController.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "TimerManager.h"

/**================================ ENEMY_BEHAVIOR ================================*/

bool ATKEnemy::TryAttack(class ABimBallCharacter* playerChar_)
{
	/* Guards */
	if (!playerChar_)
		return true;

	bool tackle = false;

	/* try to tackle if it can (not meaning that it does it, it can result in doing nothing even if it is called)*/
	if (canTackle)
		tackle = ATackleEnemy::TryAttack(playerChar_);

	/*otherwise it does a basic attack */
	if (!isTackling && canAttack)
	{
		return ABaseEnemy::TryAttack(playerChar_);
	}

	return tackle;
}

void ATKEnemy::DisableAttack_Implementation()
{
	if (canTackle)
	{
		canTackle = false;
		GetWorldTimerManager().SetTimer(tackleHandle, this, &ATKEnemy::ResetCanTackle, canTackleCoolDown);
	}

	ATackleEnemy::DisableAttack_Implementation();
}

void ATKEnemy::ResetCanTackle_Implementation()
{
	canTackle = true;
}