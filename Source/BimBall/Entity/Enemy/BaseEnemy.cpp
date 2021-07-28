// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BimBall/GameManagement/BimBallGameMode.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "AI/EnemyController.h"
#include "TimerManager.h"
#include "EnemyManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/EngineTypes.h"
#include "Components/PrimitiveComponent.h"
#include "BimBall/Entity/Enemy/BaseEnemyAnimation.h"
#include "Engine/Engine.h"

/**================================ ABASEENEMY ================================*/

ABaseEnemy::ABaseEnemy()
{
	/* Init hitZone */
	hitZone = CreateDefaultSubobject<USphereComponent>(TEXT("EnemyHitBox"));
	hitZone->InitSphereRadius(hitZoneRadius);
	hitZone->SetupAttachment(RootComponent);
}

void ABaseEnemy::BeginPlay()
{
	/* Set Ref */
	AEntity::BeginPlay();
	charaMvt		= GetCharacterMovement();
	hitZoneRadius	= hitZone->GetScaledSphereRadius();
	
	SetRunSpd();

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ABaseEnemy::OnWallHit);

}

void ABaseEnemy::OnWallHit(UPrimitiveComponent*		HitComp,
							AActor*					OtherActor, 
							UPrimitiveComponent*	OtherComp, 
							FVector					NormalImpulse, 
							const FHitResult&		hit)
{
	/* stagger on wall */
	if (shouldStaggerOnHit && FMath::Abs(hit.Normal.Z) < wallTreshold)
	{
		shouldStaggerOnHit = false;

		staggerOnHitData.knockbackDir = hit.Normal;
		Stagger(staggerOnHitData);
	}
}

void ABaseEnemy::OnHpZero_Implementation(AActor* killer)
{
	if (HasBall())
		ball->OnDropped();

	/* stop timer */
	GetWorldTimerManager().ClearTimer(statusHandle);
	GetWorldTimerManager().ClearTimer(atkHandle);
	GetWorldTimerManager().ClearTimer(spriteHandle);
	GetWorldTimerManager().ClearTimer(animHandle);

	/* disable all status */
	DisableAttack();
	ResetVulnerable();

	/* disable physics and destroy AI */
	AEnemyController* selfController = Cast<AEnemyController>(GetController());
	if (selfController)
	{
		SetActorEnableCollision(false);
		selfController->Destroy();

		/* Set death animation */
		if (UBaseEnemyAnimation* anim = Cast<UBaseEnemyAnimation>(GetMesh()->GetAnimInstance()))
		{
			anim->isDead = true;
		}
	}
}

float ABaseEnemy::RawToFinalDamages_Implementation(float rawDamages) const
{
	return isVulnerable ? rawDamages * vlnrablityAtkMultiplier : rawDamages;
}

float ABaseEnemy::Stagger(const FStaggerData& staggerData)
{
	DisableAttack();
	return Super::Stagger(staggerData);
}

/**================================ PLAYER_INTERACTION ================================*/

void ABaseEnemy::OnBallHit_Implementation(ABall* hitBall)
{
	Super::OnBallHit_Implementation(hitBall);

	SetVulnerable();
}

void ABaseEnemy::ResetVulnerable_Implementation()
{
	isVulnerable = false;
}

void ABaseEnemy::SetVulnerable_Implementation()
{
	GetWorldTimerManager().SetTimer(statusHandle, this, &ABaseEnemy::ResetVulnerable, vlnrablityDur);

	isVulnerable = true;
}

void ABaseEnemy::ApplyStagger(class ABimBallCharacter* playerChar, FStaggerData& stagger)
{
	/* Guards */
	if (!playerChar)
		return;

	FVector enemyToPlayer		= playerChar->GetActorLocation() - GetActorLocation();
	enemyToPlayer.Z				= 0.0f;
	enemyToPlayer				.Normalize();

	/* set the dir of knockback in stagger data */
	stagger.knockbackDir		= enemyToPlayer;
	playerChar					->Stagger(stagger);
}

/**================================ ENEMY_CALLBACKS ================================*/

float ABaseEnemy::GetControllerVibrationFromHit(float hitDamage, float minVibration, float maxVibration)
{
	if (hitDamage <= 0.0f)
		return minVibration;

	return FMath::Clamp(maxVibration - (1.0f/hitDamage),minVibration,maxVibration);
}

/**================================ ENEMY_BEHAVIOR ================================*/

bool ABaseEnemy::TryAttack(ABimBallCharacter* playerChar_)
{
	/* Guards */
	verify(playerChar_ != nullptr);

	TryAttackEvent();

	AEnemyController* selfController = Cast<AEnemyController>(GetController());
	/* close up the dist with player */
	if (selfController && selfController->GetToPlayer())
	{
		if (!atkHandle.IsValid())
			StartAttack(playerChar_);
	}
	/* look at player */
	SetActorRotation(FRotator(0.0f, ((playerChar_->GetActorLocation() - GetActorLocation())).Rotation().Yaw, 0.0f));

	return !canAttack;
}

bool ABaseEnemy::TryPickUp_Implementation(class ABall* ball_)
{
	AEnemyController* selfController = Cast<AEnemyController>(GetController());
	SetCatchSpd();
	/* close up the dist with ball */
	selfController->GetToBall();

	if (selfController && IsBallNear(ball_))
	{
		SetRunSpd();
		selfController->StopMovement();
		/* setup loc for the next task */
		touchDownLoc = selfController->GetClosestYard(GetActorLocation());

		ReceiveBall(ball_);

		return true;
	}

	return false;
}

void ABaseEnemy::StartAttack_Implementation(class ABimBallCharacter* playerChar_)
{
	/* attack */
	GetWorldTimerManager().SetTimer(atkHandle, FTimerDelegate::CreateUObject(this, &ABaseEnemy::Attack, playerChar_), atkDelay, false);
	/* sprite */
	GetWorldTimerManager().SetTimer(spriteHandle, this, &ABaseEnemy::SpriteEvent, spriteDelay);
	/* animation */
	GetWorldTimerManager().SetTimer(animHandle, this, &ABaseEnemy::AnimEvent, animDelay);
}

void ABaseEnemy::Attack_Implementation(ABimBallCharacter* playerChar_)
{
	/* deal damage if enemy close enough to player */
	if (playerChar_ && GetSquaredDistanceTo(playerChar_) <= hitZoneRadius * hitZoneRadius)
	{
		SetActorRotation(FRotator(0.0f, ((playerChar_->GetActorLocation() - GetActorLocation())).Rotation().Yaw, 0.0f));
		playerChar_->DealDamages(this, atk);
		ApplyStagger(playerChar_,atkStaggerData);
	}

	DisableAttack();
}

void ABaseEnemy::ResetCanAttack_Implementation()
{
	canAttack = true;
	GetWorldTimerManager().ClearTimer(atkHandle);
}

void ABaseEnemy::DisableAttack_Implementation()
{
	canAttack = false;
	/* set animation */
	if (UBaseEnemyAnimation* anim = Cast<UBaseEnemyAnimation>(GetMesh()->GetAnimInstance()))
	{
		anim->SetRun();
	}
	SetRunSpd();

	/* set for next attack attempt */
	GetWorldTimerManager().SetTimer(atkHandle, this, &ABaseEnemy::ResetCanAttack, atkCooldown);
}