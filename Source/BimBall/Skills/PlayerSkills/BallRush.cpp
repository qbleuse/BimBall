// Fill out your copyright notice in the Description page of Project Settings.


#include "BallRush.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "BimBall/Props/Ball/Ball.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBallRush::Start_Implementation() 
{
	Super::Start_Implementation();

	if (owner->GetBall() == nullptr)
	{
		TActorIterator<ABall> entityIt = TActorIterator<ABall>(GetWorld());
		while (entityIt)
		{
			/* Don't rush to the ball if not takable */
			if (!entityIt->GetIsTakable())
			{
				++entityIt;
				continue;
			}

			if ((entityIt->GetState() == EBallState::IS_DROPPED) && FMath::Square(ballSnapRangeRadius) > owner->GetSquaredDistanceTo(*entityIt))
			{
				if (UCharacterMovementComponent* charMovement = owner->GetCharacterMovement())
				{
					charMovement->Velocity = FVector::ZeroVector;
				}
				owner->SnapToActor(*entityIt);
				return;
			}

			++entityIt;
		}
	}


	End();
}