// Fill out your copyright notice in the Description page of Project Settings.


#include "TackleEnemy.h"
#include "AI/EnemyController.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "TimerManager.h"
#include "BimBall/Entity/Enemy/BaseEnemyAnimation.h"
#include "Engine/Engine.h"

/**================================ ATACKLEENEMY ================================*/

ATackleEnemy::ATackleEnemy()
{
	/* Init tackeHitZone */
	tackleHitZone = CreateDefaultSubobject<USphereComponent>(TEXT("Enemy TackleHitBox"));
	tackleHitZone->InitSphereRadius(tackleHitZoneRadius);
	tackleHitZone->SetupAttachment(RootComponent);
}

void ATackleEnemy::BeginPlay()
{
	ABaseEnemy::BeginPlay();
	tackleHitZoneRadius = tackleHitZone->GetScaledSphereRadius();
}


float ATackleEnemy::Stagger(const FStaggerData& staggerData)
{
	if (!isTackling || staggerData.forceStagger)
	{
		return Super::Stagger(staggerData);
	}

	return 0.0f;
}

/**================================ ENEMY_BEHAVIOR ================================*/

void ATackleEnemy::OnBallHit_Implementation(ABall* hitBall)
{
	if (hitBall && hitBall->IsCharged())
		isTackling = false;

	ABaseEnemy::OnBallHit_Implementation(hitBall);
}

bool ATackleEnemy::Tackle_Implementation(class ABimBallCharacter* playerChar_)
{
	AEnemyController* selfController = Cast<AEnemyController>(GetController());
	/* Guards */
	if (selfController)
	{
		FVector enemyToPlayer = (playerChar_->GetActorLocation() - GetActorLocation());

		/* there's no delay in attack on tackle */
		if (enemyToPlayer.SizeSquared() <= (tackleHitZoneRadius * tackleHitZoneRadius))
		{
			Attack(playerChar_);
			return false;
		}

		/* tackle should not do more than 90° turn when tackling */
		if (FVector::DotProduct(enemyToPlayer, GetActorForwardVector()) >= 0.0f)
		{
			float coeff = FMath::Clamp(GetWorld()->GetDeltaSeconds() * steeringStrength,0.0f,1.0f);
			tackleDir	= (tackleDir * (1.0f - coeff)) + (coeff * enemyToPlayer);

			SetActorRotation(FRotator(0.0f, (tackleDir).Rotation().Yaw, 0.0f));
		}

		/* moving */
		selfController->GetTo(GetActorLocation() + tackleDir, 0.0f);
	}

	return false;
}

void ATackleEnemy::StartTackle_Implementation()
{
	SetTackleSpd();

	/* set anim */
	if (UBaseEnemyAnimation* anim = Cast<UBaseEnemyAnimation>(GetMesh()->GetAnimInstance()))
	{
		anim->SetTKRun();
	}

	/* pause the attack */
	isTackling	= true;
	canAttack	= false;

	/* disable attack */
	GetWorldTimerManager().ClearTimer(atkHandle);
	/* disable sprite */
	GetWorldTimerManager().ClearTimer(spriteHandle);
	/* disable anim */
	GetWorldTimerManager().ClearTimer(animHandle);

	/* enable attack */
	GetWorldTimerManager().SetTimer(atkHandle, this, &ATackleEnemy::ResetCanAttack, tackleDelay);
	/* enable sprite  */
	GetWorldTimerManager().SetTimer(spriteHandle, this, &ATackleEnemy::SpriteEvent, tackleDelay);
}

bool ATackleEnemy::TryAttack(class ABimBallCharacter* playerChar_)
{
	/* Guards */
	if (!playerChar_)
		return true;

	TryAttackEvent();

	FVector enemyToPlayer = (playerChar_->GetActorLocation() - GetActorLocation());
	if (canAttack)
	{
		/* use the tackle */
		if (isTackling)
		{
			if (!atkHandle.IsValid())
				GetWorldTimerManager().SetTimer(atkHandle, this, &ATackleEnemy::DisableAttack, tackleMaxDur);
			return Tackle(playerChar_);
		}/* start the delay before tackling */
		if (!isTackling && enemyToPlayer.SizeSquared() >= GetTackleSqrdDist())
		{
			tackleDir = enemyToPlayer.GetSafeNormal();
			StartTackle();
			return false;
		}
	}

	return (!canAttack && !isTackling);
}

void ATackleEnemy::Attack_Implementation(class ABimBallCharacter* playerChar_)
{
	/* attack if tackle was used */
	if (isTackling)
	{
		FVector enemyToPlayer = (playerChar_->GetActorLocation() - GetActorLocation());

		if (FVector::DotProduct(enemyToPlayer, GetActorForwardVector()) >= 0.0f)
		{
			SetActorRotation(FRotator(0.0f,enemyToPlayer.Rotation().Yaw, 0.0f));
			playerChar_->DealDamages(this, tackleDmg);
			ApplyStagger(playerChar_, tackleStaggerData);
		}

		DisableAttack();
		return;
	}

	/* attack if basic attack was used */
	ABaseEnemy::Attack_Implementation(playerChar_);
}

void ATackleEnemy::DisableAttack_Implementation()
{
	/* cooldown are different after attacking with a basic attack and attacking with a tackle */
	if (isTackling)
		GetWorldTimerManager().SetTimer(atkHandle, this, &ATackleEnemy::ResetCanAttack, tackleCooldown);
	else
		GetWorldTimerManager().SetTimer(atkHandle, this, &ATackleEnemy::ResetCanAttack, atkCooldown);

	if (UBaseEnemyAnimation* anim = Cast<UBaseEnemyAnimation>(GetMesh()->GetAnimInstance()))
	{
		anim->SetRun();
	}

	SetRunSpd();
	canAttack	= false;
	isTackling	= false;	
}