// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicAttack.h"
#include "BimBall/Entity/Entity.h"
#include "BimBall/Entity/Enemy/BaseEnemy.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "BimBall/Dummy/DestructibleDummy.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

void UBasicAttack::Start_Implementation()
{
	Super::Start_Implementation();

	if (comboStats.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("BasicAttack : comboStats array is empty"));
	}

	UAnimInstance* instance = owner->GetMesh()->GetAnimInstance();
	instance->OnPlayMontageNotifyBegin.AddDynamic(this, &UBasicAttack::OnNotifyAttack);

	RunCurrentComboAttack();
}	

void UBasicAttack::NextCombo_Implementation()
{
	if (!isNextAttackReady)
		return;

	startTime = owner->GetWorld()->GetTimeSeconds();
	comboIndex++;

	RunCurrentComboAttack();
}

void UBasicAttack::RunCurrentComboAttack()
{
	if (!isNextAttackReady)
		return;

	verify(owner != nullptr);

	if (comboIndex >= comboStats.Num())
	{
		ResetCombo();
	}

	FComboStats& currentComboAttack = comboStats[comboIndex];

	/* Play Animation */
	if (currentComboAttack.anim)
	{
		isNextAttackReady = false;

		UAnimInstance* instance = owner->GetMesh()->GetAnimInstance();
		if (instance->Montage_IsPlaying(nullptr))
		{
			//instance->Montage_Stop(0);
		}

		currentComboAttack.anim->BlendIn = blendIn;
		currentComboAttack.anim->BlendOut = blendOut;

		//currentComboAttack.anim->bEnableAutoBlendOut = false;
		instance->Montage_Play(currentComboAttack.anim);

		//FOnMontageEnded montageEndedDelegate;
		//auto onEnd = [&](UAnimMontage*, bool) { !isNextAttackReady = false; End(); };
		//instance->Montage_SetEndDelegate(montageEndedDelegate, currentComboAttack.anim);

		//instance->OnPlayMontageNotifyBegin.AddDynamic(this, &UBasicAttack::OnNotifyAttack);
	}

	/* Select target */
	TArray<AActor*> potentialEntityTargets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEntity::StaticClass(), potentialEntityTargets);
	TArray<AActor*> potentialDummiesTargets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseDummy::StaticClass(), potentialDummiesTargets);

	TArray<AActor*> potentialActorTargets;
	potentialActorTargets += std::move(potentialEntityTargets);
	potentialActorTargets += std::move(potentialDummiesTargets);

	if (ABimBallCharacter* p = Cast<ABimBallCharacter>(owner))
	{
		FRotator rot = p->GetFollowCamera()->GetComponentRotation();
		rot.Pitch = 0;
		rot.Roll = 0;
		FVector forward = rot.RotateVector(FVector(p->joystickDirection, 0));
		
		//DrawDebugLine(
		//	GetWorld(),
		//	owner->GetActorLocation(),
		//	owner->GetActorLocation() + forward * 900,
		//	FColor(255, 0, 0),
		//	false, 5, 0,
		//	12.333
		//);
	}

	aimTarget = owner->GetAutoAimTarget(potentialActorTargets, [&](const AActor& entity) { return GetAutoAimScore(entity); });

	if (aimTarget)
	{
		FVector dir = (aimTarget->GetActorLocation() - owner->GetActorLocation()).GetSafeNormal();
		dir.Z = 0;

		/* Face Target */
		owner->SetActorRotation(FRotationMatrix::MakeFromX(FVector(dir.X, dir.Y, 0)).Rotator());

		if (owner->GetSquaredDistanceTo(aimTarget) > tpDistanceFromTarget * tpDistanceFromTarget)
		{
			/* snap to target */
			//owner->SetActorLocation(aimTarget->GetActorLocation() - dir * tpDistanceFromTarget);
			owner->SnapToLocation(aimTarget->GetActorLocation() - dir * tpDistanceFromTarget);
		}

		currentComboAttack.staggerStats.knockbackDir = dir;

		ABimBallCharacter* playerChar = Cast<ABimBallCharacter>(owner);

		if (playerChar)
			playerChar->ResetCamToClosestActor(aimTarget, playerChar);

		AEntity* enemyTarget = Cast<AEntity>(aimTarget);
		ABaseDummy* dummyTarget = Cast<ABaseDummy>(aimTarget);
		if (enemyTarget)
		{
			enemyTarget->Stagger(currentComboAttack.staggerStats);
			enemyTarget->DealDamages(owner, currentComboAttack.dmg);
		}
		if (dummyTarget)
		{
			dummyTarget->StretchOutRelativeToActor(owner);
			if (ADestructibleDummy* destructibleDummyTarget = Cast<ADestructibleDummy>(aimTarget))
			{
				destructibleDummyTarget->DealDamages(currentComboAttack.dmg);
			}
		}
		/* Warning : owner->target can now be invalid */
	}
}

void UBasicAttack::ResetCombo()
{
	comboIndex = 0;
}

float UBasicAttack::GetAutoAimScore(const AActor& target) const
{
	if (&target == owner)
		return 0;

	FScoreAutoAim scoreAimData = aimData;
	if (const ABaseEnemy* enemy = Cast<const ABaseEnemy>(&target))
	{
		if (enemy->IsDead())
			return 0;

		if (enemy->IsVulnrble())
		{
			scoreAimData.maxDist = maxDistanceToAttackIfVulnerable;
		}
	}

	if (ABimBallCharacter* p = Cast<ABimBallCharacter>(owner))
	{
		FRotator rot = p->GetFollowCamera()->GetComponentRotation();
		rot.Pitch = 0;
		rot.Roll = 0;
		FVector forward = rot.RotateVector(FVector(p->joystickDirection, 0));
		return USkill::GetAutoAimScore(*owner, target, scoreAimData, forward);
	}
	return USkill::GetAutoAimScore(target, scoreAimData);
}

void UBasicAttack::OnNotifyAttack(FName notifyName, const FBranchingPointNotifyPayload& branchingPointPayload)
{
	OnAttackNotify(notifyName);

	if (nextAttackReadyNotifyName == notifyName)
	{
		isNextAttackReady = true;

		if (ABimBallCharacter* chara = Cast<ABimBallCharacter>(owner))
		{
			chara->TryUseNextSkill();
		}
	}
	else if (stoppableCancelStartNotifyName == notifyName)
	{
		isCurrentlyStoppable = false;
	}
	else if (stoppableCancelStopNotifyName == notifyName)
	{
		isCurrentlyStoppable = true;

		if (ABimBallCharacter* chara = Cast<ABimBallCharacter>(owner))
		{
			chara->TryUseNextSkill();
		}
	}
}