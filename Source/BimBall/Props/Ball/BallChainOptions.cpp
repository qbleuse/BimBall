// Fill out your copyright notice in the Description page of Project Settings.


#include "BimBall/Props/Ball/BallChainOptions.h"
#include "BimBall/Props/Ball/Ball.h"
#include "BimBall/Entity/Enemy/BaseEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Engine.h"

void FBallChainOptions::Reset(bool isCharged)
{
	nbReboundsLeft = isCharged ? nbReboundsMax : 0;
	alreadyHitEnemies.Empty();
}

bool FBallChainOptions::Consume()
{
	if (nbReboundsLeft > 0)
	{
		--nbReboundsLeft;
		return true;
	}
	else
	{
		return false;
	}
}

AActor* FBallChainOptions::FindNextTarget(ABall* ball)
{
	TArray <TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> outActors;
	UKismetSystemLibrary::SphereOverlapActors(
		ball,
		ball->GetActorLocation(),
		reboundRadius,
		objectTypes,
		AActor::StaticClass(),
		alreadyHitEnemies,
		outActors
	);

	for (AActor* outActor : outActors)
	{
		ABaseEnemy* enemy = Cast<ABaseEnemy>(outActor);
		/* Only select alive enemies */
		if (enemy && enemy->IsAlive())
			return outActor;
	}

	return nullptr;
}