// Fill out your copyright notice in the Description page of Project Settings.


#include "Yard.h"
#include "BimBall/Entity/Enemy/BaseEnemy.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "Components/BillboardComponent.h"
#include "Engine/Engine.h"

// Sets default values
AYard::AYard()
	: Super()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	if (RootComponent)
	{
		yardZone = CreateDefaultSubobject<UBoxComponent>(TEXT("YardZone"));
		yardZone->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		

		enemyTouchdownPoint = CreateDefaultSubobject<UBillboardComponent>(TEXT("EnemyTouchdownPoint"));
		enemyTouchdownPoint->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

// Called when the game starts or when spawned
void AYard::BeginPlay()
{
	if (yardZone)
		yardZone->OnComponentBeginOverlap.AddDynamic(this, &AYard::OnYardOverlapBegin);

	Super::BeginPlay();
	
}

// Called every frame
void AYard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AYard::OnYardOverlapBegin(class UPrimitiveComponent* OverlappedComponent,
							class AActor*					OtherActor,
							class UPrimitiveComponent*		OtherComp,
							int32							OtherBodyIndex,
							bool							bFromSweep,
							const FHitResult&				SweepResult)
{

	if (ABimBallCharacter* player = Cast<ABimBallCharacter>(OtherActor))
	{
		if (player->HasBall())
			OnPlayerEnterYard(player);
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(OtherActor))
	{
		if (enemy->HasBall())
			RunEnemyTouchdown(enemy);
	}
}

void AYard::OnPlayerEnterYard_Implementation(ABimBallCharacter* player)
{

}

void AYard::RunEnemyTouchdown_Implementation(ABaseEnemy* enemy)
{
	
}

FVector AYard::GetEnemyTouchdownLocation() const
{
	verify(enemyTouchdownPoint != nullptr);
	return enemyTouchdownPoint->GetComponentLocation();
}