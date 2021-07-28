// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDummy.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "BimBall/Dummy/AnimDummy.h"
#include "BimBall/Entity/Entity.h"
#include "BimBall/Props/Ball/Ball.h"

// Sets default values
ABaseDummy::ABaseDummy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	dummyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DummyMesh"));
	SetRootComponent(dummyMesh);
	boxCollider->AttachToComponent(dummyMesh, FAttachmentTransformRules::KeepRelativeTransform, colliderAttachedSocket);
}

// Called when the game starts or when spawned
void ABaseDummy::BeginPlay()
{
	Super::BeginPlay();
	

	boxCollider->OnComponentHit.AddDynamic(this, &ABaseDummy::OnHit);

}

// Called every frame
void ABaseDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseDummy::OnBallHit_Implementation(ABall* hitBall)
{
	verify(hitBall != nullptr);
	StretchOutRelativeToActor(hitBall);
}

void ABaseDummy::StretchOut(bool downOnPositiveSide)
{
	/* Lifts up again after some seconds */
	GetWorldTimerManager().SetTimer(liftUpHandle, this, &ABaseDummy::LiftUp, liftUpDelay, false);

	/* Disables collisions with pawns, entities can go through */
	boxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	isUp = false; 

	/* Set Animation */
	if (UAnimDummy* animInstance = Cast<UAnimDummy>(dummyMesh->GetAnimInstance()))
	{
		animInstance->OnDown(downOnPositiveSide);
	}
}

void ABaseDummy::StretchOutRelativeToActor(AActor const * const actor)
{
	verify(actor != nullptr);
	/* forward and right vector are inversed for this model */
	StretchOut(FVector::DotProduct(actor->GetActorLocation() - this->GetActorLocation(), this->GetActorRightVector()) < 0);
}

void ABaseDummy::LiftUp()
{
	GetWorldTimerManager().ClearTimer(liftUpHandle);

	if (boxCollider)
	{
		TArray<AActor*> outOverlappingActors; 
		TSubclassOf<AActor> classFilter = AEntity::StaticClass();
		boxCollider->GetOverlappingActors(outOverlappingActors, classFilter);
		for (AActor* actor : outOverlappingActors)
		{
			if (AEntity* entity = Cast<AEntity>(actor))
			{
				FVector n = (entity->GetActorLocation() - this->GetActorLocation()).GetSafeNormal();
				FVector projImpulse = n;
				projImpulse.Z = 0;
				hitStaggerData.knockbackDir = (n + projImpulse).GetSafeNormal();
				entity->Stagger(hitStaggerData);
				entity->DealDamages(this, damagesOnHit);
			}
		}

		/* Enable collisions with pawns (entities) */
		boxCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	}

	isUp = true;
	if (UAnimDummy* animInstance = Cast<UAnimDummy>(dummyMesh->GetAnimInstance()))
	{
		animInstance->OnUp();
	}
}

void ABaseDummy::OnHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector normalImpulse, const FHitResult& hit)
{
	if (AEntity* entity = Cast<AEntity>(otherActor))
	{
		if (isLiftingUp)
		{
			FVector projImpulse = hit.Normal;
			projImpulse.Z = 0;
			hitStaggerData.knockbackDir = (hit.Normal + projImpulse).GetSafeNormal();
			entity->Stagger(hitStaggerData);
			entity->DealDamages(this, damagesOnHit);
		}
	}
}