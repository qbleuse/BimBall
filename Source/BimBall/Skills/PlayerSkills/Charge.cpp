// Fill out your copyright notice in the Description page of Project Settings.


#include "Charge.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BimBall/Entity/Entity.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "BimBall/Dummy/BaseDummy.h"

void UCharge::Start_Implementation()
{
	verify(owner != nullptr);
	UCharacterMovementComponent* charaMvt = owner->GetCharacterMovement();
	if (charaMvt)
	{
		previousSpeed = charaMvt->MaxWalkSpeed;
		charaMvt->MaxWalkSpeed = chargeSpeed;
	}
}

void UCharge::Tick_Implementation(float deltaTime)
{
	Super::Tick_Implementation(deltaTime);

	if (owner == nullptr)
	{
		return;
	}

	if (shouldKnockbackEnemies)
	{
		TArray<AActor*> allEntities;
		UGameplayStatics::GetAllActorsOfClass(owner, AEntity::StaticClass(), allEntities);

		for (AActor* actor : allEntities)
		{
			if (actor->GetUniqueID() != owner->GetUniqueID())
			{
				AEntity* const entity = Cast<AEntity> (actor);

				const float sqrDist = owner->GetSquaredDistanceTo(entity);
				const float sqrRadius = FMath::Square(chargeDmgRadius);
				if (sqrDist < sqrRadius)
				{
					const float ratio = (sqrRadius - sqrDist) / sqrRadius;
					const float damages = ((maxDamages - minDamages) * ratio + minDamages) * deltaTime;
					FStaggerData staggerDataApplied = staggerData;
					staggerDataApplied.knockbackDir = (entity->GetActorLocation() - owner->GetActorLocation()).GetSafeNormal();
					staggerDataApplied.duration *= ratio;
					staggerDataApplied.knockbackDistance *= ratio;
					entity->Stagger(staggerDataApplied);
					entity->DealDamages(owner, damages);
				}
			}
		}

		//TArray<ABaseDummy*> allDummies;
		//UGameplayStatics::GetAllActorsOfClass(owner, AEntity::StaticClass(), allDummies);

		//for (AActor* actor : allDummies)
		//{
		//	if (actor->GetUniqueID() != owner->GetUniqueID())
		//	{
		//		ABaseDummy* const dummy = Cast<ABaseDummy>(actor);

		//		const float sqrDist = owner->GetSquaredDistanceTo(dummy);
		//		const float sqrRadius = FMath::Square(chargeDmgRadius);
		//		if (sqrDist < sqrRadius)
		//		{
		//			dummy->StretchOut();
		//		}
		//	}
		//}
	}
}

void UCharge::End_Implementation()
{
	verify(owner != nullptr);
	UCharacterMovementComponent* charaMvt = owner->GetCharacterMovement();
	if (charaMvt)
	{
		charaMvt->MaxWalkSpeed = previousSpeed;
	}
}