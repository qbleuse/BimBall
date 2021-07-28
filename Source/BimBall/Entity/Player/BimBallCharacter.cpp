// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BimBallCharacter.h"
#include "BimBallController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Engine.h"
#include "Components/SkeletalMeshComponent.h"
#include "EngineUtils.h"
#include "Math/NumericLimits.h"
#include "TimerManager.h"
#include "BimBall/Entity/Enemy/BaseEnemy.h"
#include "BimBall/Skills/PlayerSkills/Spin.h"
#include "BimBall/Skills/PlayerSkills/BasicAttack.h"
#include "BimBall/Skills/PlayerSkills/GrabAndFling.h"
#include "BimBall/Skills/PlayerSkills/BallRush.h"
#include "BimBall/Camera/CamProfileManager.h"
#include "BimBall/Camera/CameraProfile.h"
#include "BimBall/Camera/FocusCamProfile.h"
#include "BimBall/GameManagement/BimBallGameMode.h"
#include "BimBall/Entity/Enemy/EnemyManager.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "TimerManager.h"
#include "BimBall/Skills/PlayerSkills/Charge.h"	
#include "Kismet/KismetMathLibrary.h"
#include "BimBall/BallHittable.h"
#include "BimBall/Entity/Player/BimBallCharacterAnimation.h"

/**================================ ABIMBALLCHARACTER ================================*/

ABimBallCharacter::ABimBallCharacter()
{
	/* Set size for collision capsule */
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	/* Don't rotate when the controller rotates. Let that just affect the camera.*/
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/* Configure character movement*/
	UCharacterMovementComponent* charaMvt = GetCharacterMovement();
	if (charaMvt)
	{
		charaMvt->bOrientRotationToMovement = true; // Character moves in the direction of input...	
		charaMvt->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
		charaMvt->JumpZVelocity = 600.f;
		charaMvt->AirControl = 0.2f;
	}

	/* Create a camera boom (pulls in towards the player if there is a collision)*/
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true; //Rotate the arm based on the controller

	/* Create a follow camera */
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	ballSnapRange = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerSnapRange"));
	ballSnapRange->InitSphereRadius(400);
	ballSnapRange->SetupAttachment(RootComponent);

	ballRecoveryRange->OnComponentBeginOverlap.AddDynamic(this, &ABimBallCharacter::OnBallEnterDetectionRange);
}

void ABimBallCharacter::BeginPlay()
{
	Super::BeginPlay();

	defaultRotateTurnRate = GetCharacterMovement()->RotationRate;

	if (ballSubclass.Get() != nullptr)
	{
		ReceiveBall(GetWorld()->SpawnActor<ABall>(ballSubclass.Get(), GetActorLocation(), GetActorRotation()));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BimBallCharacter : ballSubclass must be set to spawn the ball."));
	}

	ABimBallGameMode* gameMode = Cast<ABimBallGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode)
		gameMode->SetPlayerChar(this);

	SetMovementType(ECharacterMovementType::OUT_OF_FIGHT);
}

/* Called every frame */
void ABimBallCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	UpdateCoyote(deltaTime);

	if (ballRecoveryRange != nullptr && ball == nullptr && IsAlive())
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
		TArray<AActor*> actorsToIgnore;
		TArray<AActor*> outActors;
		/* Can be optimized */
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
		UKismetSystemLibrary::SphereOverlapActors(
			this,
			ballRecoveryRange->GetComponentLocation(),
			ballRecoveryRange->GetUnscaledSphereRadius(),
			objectTypes,
			ABall::StaticClass(),
			actorsToIgnore,
			outActors
		);

		for (AActor* actor : outActors)
		{
			if (ABall* b = Cast<ABall>(actor))
			{
				if (b->GetIsTakable())
				{
					this->SnapToActor(b);
					ReceiveBall(b);
				}
			}
		}
	}

	if (currentSkill)
	{
		currentSkill->Tick(deltaTime);

		if (currentSkill->IsFinished())
		{
			currentSkill = nullptr;
			TryUseNextSkill();
		}
	}

	if (chargeSkill)
	{
		chargeSkill->Tick(deltaTime);
	}

	if (GetHp() <= 0.f || (!ball.IsValid()))
	{
		AddRage(deltaTime * rageIncreasePerSec);
	}
	else if (IsRegenerating())
	{
		AddHp(deltaTime * hpRegenPerSec);
		AddRage(-deltaTime * rageDecreasePerSec);
	}

	if (isAiming)
	{
		currentBallChargeDuration += deltaTime;
		Aim();
	}

	if (selectionUIInstance != nullptr)
	{
		selectionUIInstance->SetActorRotation(
			UKismetMathLibrary::FindLookAtRotation(
				selectionUIInstance->GetActorLocation(), GetFollowCamera()->GetComponentTransform().GetLocation())
		);
	}

	UpdateMovementType();
}

/**================================ STATS ================================*/

bool ABimBallCharacter::IsRegenerating() const
{
	return UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) > lastDamageTime + healthRegenerationDelay;
}

void ABimBallCharacter::AddRage(float addedRage)
{
	rage = FMath::Clamp(rage + addedRage, 0.f, rageMax);
	if (rage == rageMax)
	{
		OnRageFull();
	}
}

bool ABimBallCharacter::HasBall() const
{
	return ball != nullptr;
}

float ABimBallCharacter::GetRage() const
{
	return rage;
}

float ABimBallCharacter::GetRageRatio() const
{
	verify(rageMax != 0)
	return rage / rageMax;
}

float ABimBallCharacter::RawToFinalDamages_Implementation(float rawDamages) const
{
	return currentSkill == nullptr ? rawDamages : currentSkill->ApplyDamagesModifiers(rawDamages);
}

/**================================ MOVEMENT ================================*/

void ABimBallCharacter::UpdateMovementType()
{
	if (GetHp() <= 0)
	{
		SetMovementType(ECharacterMovementType::DEAD);
		return;
	}
	else if (movementType != ECharacterMovementType::AIMING)
	{
		if (UBimBallCharacterAnimation* anim = Cast<UBimBallCharacterAnimation>(GetMesh()->GetAnimInstance()))
		{
			if (anim->IsTakingBall() && isTakingBallSpeedEnabled)
			{
				SetMovementType(ECharacterMovementType::TAKING_BALL);
				return;
			}
		}
		if (IsPlayerEngaged())
		{
			SetMovementType(ECharacterMovementType::IN_FIGHT);
			return;
		}
		else
		{
			SetMovementType(ECharacterMovementType::OUT_OF_FIGHT);
			return;
		}
	}
}

void ABimBallCharacter::SetMovementType(ECharacterMovementType newMovementType)
{
	movementType = newMovementType;

	if (movementType != ECharacterMovementType::DEAD && movementType != ECharacterMovementType::TAKING_BALL)
		GetCharacterMovement()->RotationRate = defaultRotateTurnRate;

	switch (movementType)
	{
	case ECharacterMovementType::OUT_OF_FIGHT:
		GetCharacterMovement()->MaxWalkSpeed = runSpeed;
		break;
	case ECharacterMovementType::IN_FIGHT:
		GetCharacterMovement()->MaxWalkSpeed = fightingSpeed;
		break;
	case ECharacterMovementType::AIMING:
		GetCharacterMovement()->MaxWalkSpeed = aimingSpeed;
		break;
	case ECharacterMovementType::DEAD:
		GetCharacterMovement()->MaxWalkSpeed = 0;
		GetCharacterMovement()->RotationRate = FRotator::ZeroRotator;
		break;
	case ECharacterMovementType::TAKING_BALL:
		GetCharacterMovement()->MaxWalkSpeed = receivingBallSpeed;
		GetCharacterMovement()->RotationRate = FRotator::ZeroRotator;
		break;
	}
}

void ABimBallCharacter::UpdateCoyote(float DeltaTime)
{
	if (UCharacterMovementComponent* charMvt = GetCharacterMovement())
	{
		if (charMvt->IsFalling() && !coyote && JumpCurrentCount == 0)
		{
			coyote			= true;
			JumpMaxCount++;
			GetWorldTimerManager().SetTimer(coyoteHandle, this, &ABimBallCharacter::ResetCoyote, DeltaTime * frameBeforeDisableJump);
		}
	}
}

void ABimBallCharacter::ResetCoyote()
{
	if (coyote)
		JumpMaxCount--;
	coyote = false;
}

void ABimBallCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	ResetCoyote();
}

void ABimBallCharacter::MoveForward(float value)
{
	if (FollowCamera)
	{
		joystickDirection.X = value;
	}

	if ((Controller != NULL) && !FMath::IsNearlyZero(value))
	{
		// get forward vector
		const FVector mvtDir = camManager ? camManager->GetFrwdVec(value) : FVector::ZeroVector;

		// add movement in that direction
		AddMovementInput(mvtDir, value);
	}
}

void ABimBallCharacter::MoveRight(float value)
{
	joystickDirection.Y = value;

	if ((Controller != NULL) && !FMath::IsNearlyZero(value))
	{
		// get right vector 
		const FVector mvtDir = camManager ? camManager->GetRightVec(value) : FVector::ZeroVector;

		// add movement in that direction
		AddMovementInput(mvtDir, value);
	}
}

void ABimBallCharacter::Charge_Implementation()
{
	if (chargeSubclass.Get() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("BimBallCharacter : chargeSubclass must be set to charge."));
	}
	chargeSkill = NewObject<USkill>(this, *chargeSubclass);
	chargeSkill->owner = this;
	if (UCharge* chargeCasted = Cast<UCharge>(chargeSkill))
	{
		chargeCasted->shouldKnockbackEnemies = IsEnraged();
	}
	chargeSkill->Start();
}

void ABimBallCharacter::StopCharging_Implementation()
{
	if (chargeSkill != nullptr)
	{
		chargeSkill->End();
		chargeSkill = nullptr;
	}
}

/**================================ CAMERA ================================*/

void ABimBallCharacter::ResetCamProfile()
{
	if (ball != nullptr && camManager)
	{
		camManager->ResetCam(GetActorRotation().Yaw, GetViewRotation().Pitch);
	}
	else if (camManager)
	{
		camManager->BallCamActivate();
	}

}

void ABimBallCharacter::ResetCamToClosestActor(AActor* actor1, AActor* actor2)
{
	if (actor1 && !actor2)
	{
		ResetCamToActor(actor1);
	}
	if (!actor1 && actor2)
	{
		ResetCamToActor(actor2);
	}	
	if (actor1 && actor2)
	{
		float dist1 = (FollowCamera->GetComponentLocation() - actor1->GetActorLocation()).SizeSquared();
		float dist2 = (FollowCamera->GetComponentLocation() - actor2->GetActorLocation()).SizeSquared();

		if (dist1 >= dist2)
		{
			
			if (camManager)
				camManager->ResetCam((actor1->GetActorLocation() - actor2->GetActorLocation()).Rotation().Yaw, GetViewRotation().Pitch);
			return;
		}

		if (camManager)
			camManager->ResetCam((actor2->GetActorLocation() - actor1->GetActorLocation()).Rotation().Yaw, GetViewRotation().Pitch);
	}
}

void ABimBallCharacter::ResetCamToActor(AActor* camTarget)
{
	if (camManager)
		camManager->ResetCam((camTarget->GetActorLocation() - GetActorLocation()).Rotation().Yaw, GetViewRotation().Pitch);
}

bool ABimBallCharacter::IsPlayerEngaged()const
{
	ABimBallGameMode* gameMode = Cast<ABimBallGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		UEnemyManager* enemyManager			= gameMode->GetEnemyManager();
		
		if (enemyManager)
		{
			const TArray<ABaseEnemy*> enemies = enemyManager->GetEnemies();

			for (const ABaseEnemy* enemy : enemies)
			{
				if ((enemy->GetActorLocation() - GetActorLocation()).SizeSquared() <= (playerEngagedDist * playerEngagedDist))
				{

					return true;
				}
			}
		}
	}
	return false;
}

void ABimBallCharacter::SetupCamManager(class UInputComponent* playerInputComponent)
{
	if (!camManager)
	{
		camManager = GetWorld()->SpawnActor<ACamProfileManager>(*subCamManager);
		camManager->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform, "Camera Manager");
		camManager->SetPlayer(this);
	}

	/* Camera Input:
	 * We have 2 versions of the rotation bindings to handle different kinds of devices differently
	 * "turn" handles devices that provide an absolute delta, such as a mouse.
	 * "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick*/
	playerInputComponent->BindAxis("Turn",			camManager,	&ACamProfileManager::Turn);
	playerInputComponent->BindAxis("TurnRate",		camManager,	&ACamProfileManager::TurnAtRate);
	playerInputComponent->BindAxis("LookUp",		camManager,	&ACamProfileManager::LookUp);
	playerInputComponent->BindAxis("LookUpRate",	camManager,	&ACamProfileManager::LookUpAtRate);
}

/**================================ ATTACK ================================*/

void ABimBallCharacter::Aim_Implementation()
{
	if (ball != nullptr)
	{
		SetIsAiming(true);

		AActor* aimTarget = GetBallAutoAimTarget();

		/* OnBallCharged / OnBallUncharged */
		bool newIsChargedAtAim = IsAimingWithChargedBall();
		if (isChargedAtAim != newIsChargedAtAim)
		{
			if (newIsChargedAtAim)
			{
				OnBallCharged();
			}

			isChargedAtAim = newIsChargedAtAim;
		}

		/* Run Animation */
		if (UBimBallCharacterAnimation* animBP = Cast<UBimBallCharacterAnimation>(GetMesh()->GetAnimInstance()))
		{
			animBP->RunAimAnim();
		}

		/* Update selectionUIInstance */
		if (selectionUIInstance == nullptr)
		{
			if (selectionUISubClass.Get() == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("BimBallCharacter : selectionUISubClass must be set"));
			}
			else
			{
				selectionUIInstance = GetWorld()->SpawnActor<AActor>(selectionUISubClass.Get(), FVector::ZeroVector, FRotator::ZeroRotator);
				selectionUIInstance->SetActorHiddenInGame(true);
			}
		}
		if (selectionUIInstance != nullptr && aimTarget != nullptr)
		{
			selectionUIInstance->AttachToActor(aimTarget, FAttachmentTransformRules::KeepRelativeTransform);
			selectionUIInstance->SetActorRelativeLocation(FVector::UpVector);
			selectionUIInstance->SetActorHiddenInGame(false);
		}
	}
}

void ABimBallCharacter::CatchNearBall()
{
	if (HasBall())
		return;

	const float currentTime = GetWorld()->GetTimeSeconds();
	if (ballRushSub.IsSkillValid() && ballRushSub.IsReady(currentTime))
	{
		ballRushSub.lastUsedTime = currentTime;

		USkill* newSkill = NewObject<USkill>(this, ballRushSub.Get());
		newSkill->owner = this;
		if (UBallRush* ballRush = Cast<UBallRush>(newSkill))
		{
			ballRush->ballSnapRangeRadius = ballSnapRange->GetUnscaledSphereRadius();
		}
		newSkill->Start();
	}

	//if (ball == nullptr)
	//{
	//	TActorIterator<ABall> entityIt = TActorIterator<ABall>(GetWorld());
	//	while (entityIt)
	//	{
	//		if (!entityIt->GetIsTakable())
	//		{
	//			++entityIt;
	//			continue;
	//		}

	//		float ballSnapRangeRadius = ballSnapRange->GetUnscaledSphereRadius();
	//		if ((entityIt->GetState() == EBallState::IS_DROPPED) && FMath::Square(ballSnapRangeRadius) > this->GetSquaredDistanceTo(*entityIt))
	//		{
	//			this->SnapToActor(*entityIt);
	//			//ReceiveBall(*entityIt);
	//			return;
	//		}

	//		++entityIt;
	//	}
	//}
}

AActor* ABimBallCharacter::GetBallAutoAimTarget() const
{
	TArray<AActor*> ballPotentialTargetes;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UBallHittable::StaticClass(), ballPotentialTargetes);
	AActor* aimTarget = GetAutoAimTarget(ballPotentialTargetes, [&](const AActor& ballTarget) { return GetBallAutoAimScore(&ballTarget); });
	if (aimTarget != nullptr)
	{
		FHitResult outHit;
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(this);
		GetWorld()->LineTraceSingleByChannel(outHit, GetActorLocation(), aimTarget->GetActorLocation(), ECollisionChannel::ECC_WorldDynamic, queryParams);

		if (outHit.GetActor() != aimTarget)
		{
			aimTarget = nullptr;
		}
	}

	return aimTarget;
}

void ABimBallCharacter::SetIsAiming(bool newValue)
{
	if (isAiming != newValue)
	{
		SetMovementType(newValue ? ECharacterMovementType::AIMING : ECharacterMovementType::OUT_OF_FIGHT);

		isAiming = newValue;

		if (isAiming)
		{
			OnAimStart();
		}
		else
		{
			OnAimEnd();

			currentBallChargeDuration = 0;

			/* Run Animation / Cancel Aim */
			if (UBimBallCharacterAnimation* animBP = Cast<UBimBallCharacterAnimation>(GetMesh()->GetAnimInstance()))
			{
				animBP->SetIdle();
			}

			if (isChargedAtAim)
			{
				OnBallUncharged();
				isChargedAtAim = false;
			}
		}
	}
}

void ABimBallCharacter::OnBallEnterDetectionRange(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (ABall* overlappedBall = Cast<ABall>(OtherActor))
	{
		if (currentSkill == nullptr && chargeSkill == nullptr && overlappedBall->GetIsTakable())
		{
			snapTimeline.Stop();
			this->SnapToActor(overlappedBall);
			ReceiveBall(overlappedBall);
		}
	}
}

void ABimBallCharacter::DropBall_Implementation()
{
	SetIsAiming(false);

	UCharacterMovementComponent* charMvt = GetCharacterMovement();
	if (ball != nullptr && charMvt && !charMvt->IsFalling())
	{
		ball->OnDropped(GetActorRotation().RotateVector(dropDir) * dropImpulse);
		ball = nullptr;

		GetWorldTimerManager().SetTimer(enragedHandle, [&]() { SetEnraged(true); }, enragedCooldownAfterDroppingBall, false);
	}
}

void ABimBallCharacter::Throw_Implementation()
{
	if (!HasBall())
		return;

	AActor* aimTarget = GetBallAutoAimTarget();

	const bool isCharging = currentBallChargeDuration > ballChargeDuration;

	SetIsAiming(false);

	/* Run Animation */
	if (UBimBallCharacterAnimation* animBP = Cast<UBimBallCharacterAnimation>(GetMesh()->GetAnimInstance()))
	{
		animBP->RunThrowAnim();
	}

	if (aimTarget != nullptr)
	{
		ball->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ball->OnThrown(this, aimTarget, isCharging);
		ball = nullptr;

		ResetCamToClosestActor(aimTarget,this);
	}

	if (selectionUIInstance)
		selectionUIInstance->SetActorHiddenInGame(true);
}

void ABimBallCharacter::ReceiveBall_Implementation(ABall* b)
{
	
	if (!b->GetIsTakable())
		return;

	if (b != nullptr && GetMesh() != nullptr)
	{
		if (camManager && camManager->isBallCamActivated)
			camManager->BallCamActivate();
			

		GetWorldTimerManager().ClearTimer(enragedHandle);
		SetEnraged(false);

		ABimBallGameMode* gameMode = Cast<ABimBallGameMode>(GetWorld()->GetAuthGameMode());
		if (gameMode && gameMode->GetEnemyManager())
		{
			UEnemyManager* enemyManager = gameMode->GetEnemyManager();
			if (enemyManager->ballBeingCarried)
			{
				AddHp(hpMax);
				enemyManager->ballBeingCarried = false;
			}
		}

		SetMovementType(ECharacterMovementType::TAKING_BALL);

		Super::ReceiveBall_Implementation(b);

		if (currentSkill != nullptr)
		{
			if (UGrabAndFling* grabAndFling = Cast<UGrabAndFling>(currentSkill))
			{
				FlingEntity();
			}
		}
	}
}

void ABimBallCharacter::SetEnraged(bool newIsEnragedValue)
{
	isEnraged = newIsEnragedValue;

	if (chargeSkill)
	{
		if (UCharge* c = Cast<UCharge>(chargeSkill))
		{
			c->shouldKnockbackEnemies = isEnraged;
		}
	}
}

float ABimBallCharacter::GetBallAutoAimScore(const AActor* ballTarget) const
{
	if (ballTarget == this)
		return 0;

	if (const AEntity* entity = Cast<const AEntity>(ballTarget))
	{
		/* if entity is dead or not being rendered */
		if (entity->IsDead() || !entity->GetMesh() || ((GetWorld()->GetTimeSeconds() - entity->GetMesh()->GetLastRenderTimeOnScreen()) >= disableAimTime))
			return 0;

		FRotator rot			= FollowCamera->GetComponentRotation();
		rot.Pitch				= 0;
		rot.Roll				= 0;
		FVector v				= joystickDirection.IsNearlyZero() ? GetActorForwardVector() : rot.RotateVector(FVector(joystickDirection, 0));
		FVector entityToPlayer	= (entity->GetActorLocation() - GetActorLocation());

		if (v.Normalize() && entityToPlayer.Normalize())
		{
			float acos = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(v, entityToPlayer)));
			if (acos >= -aimLimitAngle && acos <= aimLimitAngle)
				return USkill::GetAutoAimScore(*this, *ballTarget, aimData, v);
		}
	}

	return 0;
}

void ABimBallCharacter::Spin_Implementation()
{
	if (!TryUseSkill(spinSub))
	{
		nextSkill = &spinSub;
	}
}

bool ABimBallCharacter::TryBasicAttack_Implementation()
{
	if (currentSkill != nullptr)
	{
		if (UBasicAttack* basicAttack = Cast<UBasicAttack>(currentSkill))
		{
			if (basicAttack->IsNextAttackAvailable())
			{
				basicAttack->NextCombo();
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	
	return TryUseSkill(basicAttackSub);
}

void ABimBallCharacter::BasicAttack()
{
	if (!TryBasicAttack())
	{
		nextSkill = &basicAttackSub;
	}
}

void ABimBallCharacter::GrabEntityAuto()
{
	if (!grabAndFlingSub.IsSkillValid())
		return;

	if (currentSkill == nullptr || currentSkill->IsStoppable())
	{
		currentSkill = NewObject<USkill>(this, grabAndFlingSub.subClass.Get());
		currentSkill->owner = this;
		if (UGrabAndFling* grabAndFling = Cast<UGrabAndFling>(currentSkill))
		{
			TArray<AActor*> potentialEntityTargets;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), potentialEntityTargets);
			ABaseEnemy* targetEnemy = Cast<ABaseEnemy>(GetAutoAimTarget(potentialEntityTargets, [&](const AActor& entity) { return grabAndFling->GetAutoAimScore(entity); }));

			if (!targetEnemy || !targetEnemy->IsVulnrble())
			{
				currentSkill	= nullptr;
			}

			grabAndFling->target = targetEnemy;
		}

		if (currentSkill)
		{
			currentSkill->Start();
		}
	}
}

void ABimBallCharacter::GrabEntity_Implementation(AEntity* grabbedEntity)
{
	if (currentSkill == nullptr || currentSkill->IsStoppable())
	{
		currentSkill = NewObject<USkill>(this, *grabAndFlingSub);
		currentSkill->owner = this;
		if (UGrabAndFling* grabAndFling = Cast<UGrabAndFling>(currentSkill))
		{
			grabAndFling->target = grabbedEntity;
		}
		currentSkill->Start();
	}
}

void ABimBallCharacter::FlingEntity_Implementation()
{
	if (UGrabAndFling* grabAndFling = Cast<UGrabAndFling>(currentSkill))
	{
		grabAndFling->End();
		currentSkill = nullptr;
	}
}

bool ABimBallCharacter::TryUseSkill(FSkillWithCooldown& skillSubClass)
{
	SetIsAiming(false);

	if (!skillSubClass.IsSkillValid())
	{
		UE_LOG(LogTemp, Error, TEXT("BimBallCharacter : skillSubClass must be valid."));
		return false;
	}

	float currentTime = GetWorld()->GetTimeSeconds();

	if ((currentSkill == nullptr || currentSkill->IsStoppable()) && skillSubClass.IsReady(currentTime))
	{
		skillSubClass.lastUsedTime = currentTime;

		USkill* newSkill = NewObject<USkill>(this, skillSubClass.Get());
		newSkill->owner = this;
		newSkill->Start();
		currentSkill = newSkill;
		return true;
	}
	return false;
}

void ABimBallCharacter::TryUseNextSkill()
{
	if (!nextSkill)
		return;

	if (nextSkill == &basicAttackSub)
	{
		if (TryBasicAttack())
		{
			nextSkill = nullptr;
		}
	}
	else
	{
		if (TryUseSkill(*nextSkill))
		{
			nextSkill = nullptr;
		}
	}
}

float ABimBallCharacter::Stagger(const FStaggerData& staggerData) 
{
	USpin* spin = Cast<USpin>(currentSkill);
	if (spin == nullptr)
	{
		Super::Stagger(staggerData);

		SetIsAiming(false);

		float staggerDur = Super::Stagger(staggerData);

		if (ABimBallController* playerController = Cast<ABimBallController>(Controller))
		{
			playerController->IncapacitateFor(staggerDur);
		}
		return staggerDur;
	}
	return 0.0f;
}

void ABimBallCharacter::DealDamages_Implementation(AActor* attacker, float rawDamages)
{
	SetActorRotation(FRotator(0.0f, ((attacker->GetActorLocation() - GetActorLocation())).Rotation().Yaw, 0.0f));
	Super::DealDamages_Implementation(attacker, rawDamages);

	lastDamageTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
}

void ABimBallCharacter::OnHpZero_Implementation(AActor* killer)
{
	verify(FMath::IsNearlyZero(GetHp()));

	ABimBallGameMode* gameMode = Cast<ABimBallGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode && gameMode->GetEnemyManager())
	{
		UEnemyManager* enemyManager = gameMode->GetEnemyManager();
		if (enemyManager->ballBeingCarried || (enemyManager->GetBall() && enemyManager->GetBall()->GetState() == EBallState::IS_DROPPED))
		{
			AddHp(0.1f);
			return;
		}
	}

	if (ball != nullptr)
	{
		ball->OnDropped(FVector::ZeroVector);
		ball = nullptr;
	}

	GetWorldTimerManager().SetTimer(enragedHandle, [&]() { SetEnraged(true); }, enragedCooldownAfterDroppingBall, false);

	if (ABimBallController* playerController = Cast<ABimBallController>(Controller))
	{
		playerController->IncapacitateFor(incapacitationDuration);
	}

	/* Run Animation */
	if (UBimBallCharacterAnimation* animBP = Cast<UBimBallCharacterAnimation>(GetMesh()->GetAnimInstance()))
	{
		animBP->SetIncapacitated();

		GetWorldTimerManager().SetTimer(incapacitatedHandle, [&, animBP]()
			{
				animBP->SetIdle();
				AddHp(0.1f);
			}, incapacitationDuration, false);
	}
	
}