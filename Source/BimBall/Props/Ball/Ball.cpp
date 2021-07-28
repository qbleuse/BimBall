// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball.h"
#include "BimBall/Entity/Entity.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "BimBall/Entity/Enemy/BaseEnemy.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "BimBall/Entity/Entity.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"

// Sets default values
ABall::ABall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	ballMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	RootComponent = ballMesh;
}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (target == nullptr)
		return;

	velocity = (target->GetActorLocation() - this->GetActorLocation()).GetSafeNormal() * speed;

	FVector newLoc = GetActorLocation() + velocity;

	if (FVector::DistSquared(GetActorLocation(), newLoc) >= FVector::DistSquared(target->GetActorLocation(), this->GetActorLocation()))
	{
		SetActorLocation(target->GetActorLocation());
	}
	else
	{
		SetActorLocation(GetActorLocation() + velocity);
	}

	if (this->GetSquaredDistanceTo(target.Get()) < FMath::Square(targetHitDist + velocity.Size()))
	{
		OnTargetHit();
	}
}


void ABall::OnThrown_Implementation(AEntity* thrower, AActor* newTarget, bool isThrowCharged)
{
	this->ownerCharacter = thrower;
	this->target = newTarget;

	chainOptions.Reset(isThrowCharged);

	/* Make the ball not takable for some time */
	isTakable = false;
	GetWorldTimerManager().SetTimer(isTakableHandler, TFunction<void()>([&]() { isTakable = true; }), dropUntakableDuration, false);
}

void ABall::OnDropped_Implementation(FVector impulse)
{

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	ballMesh->SetSimulatePhysics(true);
	if (ownerCharacter.Get())
		ballMesh->SetAllPhysicsLinearVelocity(ownerCharacter->GetVelocity());

	ownerCharacter = nullptr;

	if (impulse.SizeSquared() == 0.0f)
		ballMesh->AddImpulse(UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector::UpVector, dropRandomHalfAngleInDegrees) * dropImpulseLength);
	else
		ballMesh->AddImpulse(impulse);

	/* Make the ball not takable for some time */
	isTakable = false;
	GetWorldTimerManager().SetTimer(isTakableHandler, TFunction<void()>([&]() { isTakable = true; }), dropUntakableDuration, false);
}

void ABall::OnReceived_Implementation(AEntity* newOwner)
{
	ownerCharacter	= newOwner;
	target			= nullptr;
	ballMesh->SetSimulatePhysics(false);
}

void ABall::OnTargetHit_Implementation()
{
	if (ballMesh == nullptr)
		return;

	if (ownerCharacter == target)
	{
		ownerCharacter->ReceiveBall(this);
		velocity = FVector::ZeroVector;
		target = nullptr;
	}
	else
	{
		chainOptions.alreadyHitEnemies.Add(target.Get());
		if (target->GetClass()->ImplementsInterface(UBallHittable::StaticClass()))
		{
			IBallHittable::Execute_OnBallHit(target.Get(), this); // Context, Ball
		}

		velocity *= -1.f;
		if (ownerCharacter != nullptr && target != nullptr)
		{
			velocity.Z += ownerCharacter->GetDistanceTo(target.Get()) / 4;
		}

		bool isThrown = true;
			
		if (chainOptions.Consume())
		{
			target = chainOptions.FindNextTarget(this);
			isThrown = target == nullptr;
		}

		if (isThrown)
		{
			ballMesh->SetSimulatePhysics(true);
			if (ownerCharacter != nullptr)
			{
				const FVector dir = (ownerCharacter->GetActorLocation())
					- this->GetActorLocation();
				FVector randDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(dir, reboundAngle) * reboundScale;
				randDir.Z = reboundUp;
				if (shouldReboundBeOpposite)
				{
					randDir.X *= -1;
					randDir.Y *= -1;
				}
				ballMesh->SetPhysicsLinearVelocity(randDir + ownerCharacter->GetVelocity() * reboundFollowVelocityPower);
			}

			target = nullptr;
			ownerCharacter = nullptr;
		}
	}
}


EBallState ABall::GetState() const
{
	if (ownerCharacter == nullptr)
	{
		return EBallState::IS_DROPPED;
	}
	else
	{
		if (target == nullptr)
		{
			return EBallState::IS_CARRIED;
		}
		else if (target == ownerCharacter)
		{
			return EBallState::IS_RETURNING;
		}
		else
		{
			return EBallState::IS_THROWN;
		}
	}
}

void ABall::UpdateGroundIndicatorLocation(USceneComponent* ballGroundIndicator, ECollisionChannel traceChannel)
{
	if (!ballGroundIndicator)
		return;

	verify(GetWorld() != nullptr);

	FVector start = GetActorLocation();
	FVector end = start + FVector::DownVector * 1000;

	FHitResult outHit;
	FCollisionQueryParams collisionParams;

	collisionParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(outHit, start, end, traceChannel, collisionParams);

	float offset = 5;
	ballGroundIndicator->SetWorldLocation(outHit.Location + FVector::UpVector * offset);
}

void ABall::UpdateGroundIndicator(class USceneComponent* ballGroundIndicator, ECollisionChannel traceChannel)
{
	if (ballGroundIndicator == nullptr)
		return;

	bool isNotInOwnerHands = !IsInOwnerHands();
	ballGroundIndicator->SetVisibility(isNotInOwnerHands, false);
	if (isNotInOwnerHands)
	{
		UpdateGroundIndicatorLocation(ballGroundIndicator, traceChannel);
	}
}

bool ABall::IsInOwnerHands() const
{
	return ownerCharacter != nullptr && ownerCharacter->GetBall() != nullptr;
}

bool ABall::IsBallOutOfView(const FVector& viewLocation) const
{
	FVector start = GetActorLocation();
	FVector end = viewLocation;

	FHitResult outHit;
	FCollisionQueryParams collisionParams;

	collisionParams.AddIgnoredActor(this);
	collisionParams.AddIgnoredActor(ballSeeThroughIndicator);
	collisionParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(this, 0));

	GetWorld()->LineTraceSingleByChannel(outHit, 
										GetActorLocation(), 
										viewLocation, 
										ECC_Visibility, 
										collisionParams);

	return outHit.bBlockingHit;
}

bool ABall::IsBallOutOfCameraView() const
{
	if (APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0))
		return IsBallOutOfView(cameraManager->GetCameraLocation());
	else
		return false;
}

void ABall::UpdateBallSeeThrough()
{
	bool newIsSeeThrough;
	if (APawn* playerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		newIsSeeThrough = IsBallOutOfCameraView() || GetSquaredDistanceTo(playerPawn) >= FMath::Square(seeThroughDistanceTreshold);
	}
	else
	{
		newIsSeeThrough = false;
	}

	if (newIsSeeThrough != isSeeThrough)
	{
		if (newIsSeeThrough)
		{
			EnableBallSeeThrough();
		}
		else
		{
			DisableBallSeeThrough();
		}
	}

	isSeeThrough = newIsSeeThrough;
}

bool ABall::ComputeBallScreenLocation(FVector2D& outputLoc) const
{
	if (APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0))
	{
		FVector camWorldLocation;
		FVector camWorldDirection;
		controller->DeprojectScreenPositionToWorld(ballIndicatorBounds.X, 
													ballIndicatorBounds.Y,
													camWorldLocation,
													camWorldDirection);

		FVector forwardWorldLoc = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::FindLookAtRotation(camWorldLocation, GetActorLocation())) + camWorldLocation;
		return (controller->ProjectWorldLocationToScreen(forwardWorldLoc, outputLoc, false));
	}
	return false;
}

float ABall::ComputeBallIndicatorAngle(const FVector2D& ballIndicatorLocation) const
{
	const FVector2D n = ballIndicatorLocation.GetSafeNormal();
	
	const float value = FMath::RadiansToDegrees(acos(n.X));

	return 90 + (n.Y >= 0 ? value : -value);
}

FVector2D ABall::ComputeBallIndicatorLocation(const FVector2D& targetScreenLoc) const
{ 
	FVector2D offsetLoc = targetScreenLoc - ballIndicatorBounds;
	FVector2D div = offsetLoc / FMath::Abs(offsetLoc.X + offsetLoc.Y);

	return div * ((ballIndicatorBounds / div).GetAbs().GetMin());
}

FVector2D ABall::ComputeBallIndicatorBounds() const 
{ 
	FIntPoint resolution = UGameUserSettings::GetGameUserSettings()->GetScreenResolution();
	return (FVector2D(resolution) / 2.0) - ballIndicatorOffset;

}