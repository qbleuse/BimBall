// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity.h"
#include "BimBall/Props/Ball/Ball.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "EngineUtils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BimBall/Entity/Player/BimBallCharacterAnimation.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values
AEntity::AEntity()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* Init ballRecoveryRange */
	ballRecoveryRange = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerRecoveryRange"));
	ballRecoveryRange->InitSphereRadius(150);
	ballRecoveryRange->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEntity::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat timelineCallback;
	if (snapCurve)
	{
		timelineCallback.BindUFunction(this, FName("OnSnapTick"));
		snapTimeline.AddInterpFloat(snapCurve, timelineCallback);
	}
}

// Called every frame
void AEntity::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (snapCurve)
		snapTimeline.TickTimeline(deltaTime);
}

void AEntity::OnBallHit_Implementation(ABall* hitBall)
{
	hitBall->hitStaggerData.knockbackDir = (this->GetActorLocation() - hitBall->GetActorLocation()).GetSafeNormal();
	hitBall->hitStaggerData.knockbackDir.Z = 0;
	Stagger(hitBall->hitStaggerData);
	DealDamages(hitBall->GetOwnerCharacter(), hitBall->GetDamages());
}

void AEntity::ReceiveBall_Implementation(ABall* b)
{
	if (!b->GetIsTakable())
		return;

	if (UBimBallCharacterAnimation* anim = Cast<UBimBallCharacterAnimation>(GetMesh()->GetAnimInstance()))
	{
		anim->UpdateBallRelativeLoc(b);
		anim->SetTakingBall();
	}

	b->OnReceived(this);
	b->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, ballAttachedSocketName);
	b->SetActorRelativeLocation(FVector::ZeroVector);
	this->ball = b;
}

bool AEntity::IsBallNear(const ABall* testedBall) const
{
	if (testedBall == nullptr || ballRecoveryRange == nullptr)
		return false;
	
	return GetSquaredDistanceTo(testedBall) < FMath::Square(ballRecoveryRange->GetScaledSphereRadius());
}

void AEntity::DealRawDamages_Implementation(AActor* attacker, float damages)
{
	if (IsDead())
		return;

	hp = FMath::Max(0.f, hp - damages);

	if (IsDead())
	{
		OnHpZero(attacker);
	}
}

void AEntity::DealDamages_Implementation(AActor* attacker, float rawDamages)
{
	DealRawDamages(attacker, RawToFinalDamages(rawDamages));
}

void AEntity::OnHpZero_Implementation(AActor* killer)
{
	this->Destroy();
}

float AEntity::RawToFinalDamages_Implementation(float rawDamages) const
{
	return rawDamages;
}

void AEntity::OnSnapTick()
{
	if (snapCurve != nullptr)
	{
		FVector lerpValue = FMath::Lerp(locBeforeSnap, snapTargetLoc, snapCurve->GetFloatValue(snapTimeline.GetPlaybackPosition()));
		if (lerpValue.Z < locBeforeSnap.Z && GetCharacterMovement()->IsMovingOnGround())
			lerpValue.Z = locBeforeSnap.Z;
		SetActorLocation(lerpValue, true);
	}
}

void AEntity::SnapToActor(AActor* snapTarget) 
{
	verify(snapTarget != nullptr);

	SnapToLocation(snapTarget->GetActorLocation());
}

void AEntity::SnapToLocation_Implementation(const FVector& snapLocation)
{
	//this->SetActorLocation(snapLocation);

	locBeforeSnap = GetActorLocation();
	snapTargetLoc = snapLocation;
	snapTimeline.PlayFromStart();
}

/**================================ STATS ================================*/

void AEntity::AddHp(float addedHp)
{
	verify(addedHp);
	hp = FMath::Min(hp + addedHp, hpMax);
}

float AEntity::GetHp() const
{
	return hp;
}

float AEntity::GetHpRatio() const
{
	verify(hpMax != 0)
	return hp / hpMax;
}

float AEntity::Stagger(const FStaggerData& staggerData)
{
	float currentTime = GetGameTimeSinceCreation();
	staggerDuration -= currentTime - lastStaggerTime;
	lastStaggerTime = currentTime;
	staggerDuration = FMath::Max(staggerDuration, staggerData.duration * staggerData.durationMultiplier);

	const FVector vec = (staggerData.knockbackDir * staggerData.knockbackDistance * staggerData.knockbackDistanceMultiplier);
	const FVector n = FVector::CrossProduct(vec, FVector::UpVector).GetSafeNormal();
	const FVector rotatedVec = UKismetMathLibrary::RotateAngleAxis(vec, staggerData.knockbackAngle, n);
	this->GetCharacterMovement()->AddImpulse(rotatedVec, true);
	this->GetCharacterMovement()->MovementState.bCanJump = false;

	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, TFunction<void()>([&]() 
	{ 
		this->GetCharacterMovement()->MovementState.bCanJump = true;
	}), staggerDuration, false);

	return PlayAnimMontage(hitAnim);
}

bool AEntity::IsStaggered() const
{
	return GetGameTimeSinceCreation() < lastStaggerTime + staggerDuration;
}

/**================================ AIM ================================*/

AActor* AEntity::GetAutoAimTarget(const TArray<AActor*>& potentialTargets, const TFunction<float(const AActor& actor)>& aimScoreGetter) const
{
	verify(aimScoreGetter);

	AActor* returnedActor = nullptr;
	float maxScore = 0;
	for (AActor* actor : potentialTargets)
	{
		const float score = aimScoreGetter(*actor);
		if (score > maxScore)
		{
			maxScore = score;
			returnedActor = actor;
		}
	}

	return returnedActor;
}
//
//float ABimBallCharacter::GetAutoAimScore(AEntity* entity) const
//{
//	verify(entity != nullptr)
//
//		if (entity == this)
//			return 0;
//
//	const float distScore = autoAimDistScoreScale / (entity->GetDistanceTo(this) + 1.f);
//
//	const float inFrontScore = autoAimInFrontScoreScale * FVector::DotProduct(this->GetActorForwardVector(), (entity->GetActorLocation() - GetActorLocation()).GetSafeNormal());
//
//	return distScore + inFrontScore;
//}
//
