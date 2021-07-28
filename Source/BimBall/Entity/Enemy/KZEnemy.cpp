// Fill out your copyright notice in the Description page of Project Settings.


#include "KZEnemy.h"
#include "AI/EnemyController.h"
#include "TimerManager.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "BimBall/Dummy/BaseDummy.h"
#include "BimBall/Entity/Enemy/BaseEnemyAnimation.h"
#include "Engine/Engine.h"

/**================================ ENEMY_BEHAVIOR ================================*/

bool AKZEnemy::TryAttack(class ABimBallCharacter* playerChar_)
{
	/* Guards */
	if (!playerChar_)
		return true;

	if (UBaseEnemyAnimation* anim = Cast<UBaseEnemyAnimation>(GetMesh()->GetAnimInstance()))
	{
		anim->SetKZRun();
	}

	if (useTackleBehavior)
		return ATackleEnemy::TryAttack(playerChar_);

	return ABaseEnemy::TryAttack(playerChar_);
}

void AKZEnemy::Attack_Implementation(class ABimBallCharacter* playerChar_)
{
	if (!isVulnerable)
		Explode();
	else
		DisableAttack();
}

void AKZEnemy::OnHpZero_Implementation(AActor* killer)
{
	if (!isVulnerable)
		GetWorldTimerManager().SetTimer(atkHandle, this, &AKZEnemy::Explode, atkDelay);
	else
	{
		ABaseEnemy::OnHpZero_Implementation(killer);
	}
}

void AKZEnemy::Explode_Implementation()
{
	TArray<AActor*> outOverlap;
	hitZone->GetOverlappingActors(outOverlap);

	for (AActor* actor : outOverlap)
	{
		if (actor)
		{
			AEntity* entity = Cast<AEntity>(actor);
			if (entity)
			{
				entity->DealDamages(this, atk);
			}
			ABaseDummy* dummy = Cast<ABaseDummy>(actor);
			if (dummy)
			{
				dummy->StretchOutRelativeToActor(this);
			}

		}
	}

	if (onDeathSpawnedActor)
	{
		GetWorld()->SpawnActor<AActor>(*onDeathSpawnedActor, GetTransform());
	}

	ABaseEnemy::OnHpZero_Implementation(this);
}

void AKZEnemy::OnBallHit_Implementation(ABall* hitBall)
{
	Super::OnBallHit_Implementation(hitBall);

	if (hitBall && hitBall->IsCharged())
		GetWorldTimerManager().SetTimer(atkHandle, this, &AKZEnemy::Explode, atkDelay);
	else
		GetWorldTimerManager().ClearTimer(atkHandle);
}