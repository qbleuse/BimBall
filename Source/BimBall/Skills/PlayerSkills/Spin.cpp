// Fill out your copyright notice in the Description page of Project Settings.


#include "Spin.h"

#include "BimBall/Entity/Entity.h"
#include "Kismet/GameplayStatics.h"
#include "BimBall/Entity/Entity.h"
#include "BimBall/Entity/Enemy/BaseEnemy.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"

void USpin::DealDamages()
{
	verify(owner != nullptr);
	verify(maxDamages >= minDamages)

	TActorIterator<AEntity> entityIt = TActorIterator<AEntity>(owner->GetWorld());

	while (entityIt)
	{
		if (entityIt->GetUniqueID() != owner->GetUniqueID())
		{
			if (entityIt->IsDead())
			{
				++entityIt;
				continue;
			}

			const float sqrDist = owner->GetSquaredDistanceTo(*entityIt);
			const float sqrRadius = FMath::Square(spinRadius);
			if (sqrDist < sqrRadius)
			{
				const float ratio = (sqrRadius - sqrDist) / sqrRadius;
				const float damages = (maxDamages - minDamages) * ratio + minDamages;

				/* Knockback on Wall hit*/
				if (ABaseEnemy* enemy = Cast<ABaseEnemy>(*entityIt))
				{
					enemy->StaggerOnHit(staggerDataOnHit);
					enemy->SetVulnerable();
				}

				/* Knockback */
				FVector ownerToTarget = entityIt->GetActorLocation() - owner->GetActorLocation();
				ownerToTarget.Z = 0; // make it on the plane x y 
				ownerToTarget.Normalize();
				stagger.knockbackDir = ownerToTarget;
				entityIt->Stagger(stagger);

				/* Damages */
				entityIt->DealDamages(owner, damages);
			}
		}

		++entityIt;
	}

}
//#include "BimBall/Entity/Player/BimBallCharacterAnimation.h"
void USpin::Start_Implementation() 
{
	Super::Start_Implementation();

	if (anim)
	{
		/* DealDamages() function should then be called by a notify */
		owner->GetMesh()->GetAnimInstance()->Montage_Play(anim);
	}

	if (ABimBallCharacter* player = Cast<ABimBallCharacter>(owner))
	{
		player->SetSpinCooldown(spinDefaultCooldown);
	}
}

float USpin::ApplyDamagesModifiers_Implementation(float rawDamages) const
{
	if (ABimBallCharacter* player = Cast<ABimBallCharacter>(owner))
	{
		player->SetSpinCooldown(spinShorterCooldown);
	}

	return 0;
}