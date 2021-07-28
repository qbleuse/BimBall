// Fill out your copyright notice in the Description page of Project Settings.


#include "Bumper.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BimBall/Entity/Entity.h"

// Sets default values
ABumper::ABumper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

// Called when the game starts or when spawned
void ABumper::BeginPlay()
{
	Super::BeginPlay();

	mesh->OnComponentBeginOverlap.AddDynamic(this, &ABumper::OnOverlapBegin);
	
}

void ABumper::OnOverlapBegin(class UPrimitiveComponent* overlappedComp, 
							 class AActor* otherActor,
							 class UPrimitiveComponent* otherComp,
							 int32 otherBodyIndex,
							 bool bFromSweep,
							 const FHitResult& sweepResult)
{
	if (AEntity* player = Cast<AEntity>(otherActor))
	{
		player->GetCharacterMovement()->AddImpulse(impulse, true);
	}
}



