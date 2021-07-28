// Fill out your copyright notice in the Description page of Project Settings.

#include "GrabAndFling.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BimBall/Entity/Entity.h"
#include "Components/SkeletalMeshComponent.h"
#include "BimBall/Entity/Enemy/BaseEnemy.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "BimBall/Entity/Player/BimBallCharacterAnimation.h"

void UGrabAndFling::Start_Implementation()
{
	if (owner != nullptr && target.IsValid() 
		&& owner->GetSquaredDistanceTo(target.Get()) < FMath::Square(maxDistanceToAttack))
	{
		/* Play Animation */
		if (UBimBallCharacterAnimation* animInstance = Cast<UBimBallCharacterAnimation>(owner->GetMesh()->GetAnimInstance()))
		{
			animInstance->RunGrabAnim();
		}

		FAttachmentTransformRules attachment = FAttachmentTransformRules::KeepWorldTransform;
		target->AttachToComponent(owner->GetMesh(), attachment, attachmentBone);
		target->SetActorRelativeLocation(FVector::ZeroVector);
		target->SetActorRelativeRotation(FRotator::ZeroRotator);

		//USkeletalMeshComponent* skMeshComp = target->GetMesh();
		//skMeshComp->AttachToComponent(owner->GetMesh(), attachment, attachmentBone);
		//skMeshComp->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);

		target->SetActorEnableCollision(false);
		if (ACharacter* targetChar = Cast<ACharacter>(target))
		{
			targetChar->GetCharacterMovement()->Velocity = FVector::ZeroVector;
			targetChar->GetCharacterMovement()->GravityScale = 0.f;
			//target->GetMesh()->SetSimulatePhysics(true);
		}
	}
	else
	{
		End();
	}
}

void UGrabAndFling::Tick_Implementation(float deltaTime)
{
	if (owner != nullptr && target.IsValid())
	{
		if (ABaseEnemy* baseEnemy = Cast<ABaseEnemy>(target.Get()))
		{
			baseEnemy->SetVulnerable();
		}
	}
}
   

void UGrabAndFling::End_Implementation()
{
	if (owner != nullptr && target.IsValid())
	{
		/* Play Animation */
		if (UBimBallCharacterAnimation* animInstance = Cast<UBimBallCharacterAnimation>(owner->GetMesh()->GetAnimInstance()))
		{
			animInstance->RunFlingAnim();
		}

		FDetachmentTransformRules attachment = FDetachmentTransformRules::KeepWorldTransform;
		if (ACharacter* targetChar = Cast<ACharacter>(target))
		{
			targetChar->GetCharacterMovement()->AddImpulse(owner->GetActorForwardVector() * knockbackImpulse + owner->GetActorUpVector() * upKnockbackImpulse);
			targetChar->GetCharacterMovement()->GravityScale = 1.f;
		}
		target->DetachFromActor(attachment);
		target->SetActorEnableCollision(true);

		if (ABaseEnemy* baseEnemy = Cast<ABaseEnemy>(target))
		{
			baseEnemy->StaggerOnHit(staggerDataOnHit);
			baseEnemy->SetVulnerable();
		}
	}
}

float UGrabAndFling::GetAutoAimScore(const AActor& aimTarget) const
{
	if (&aimTarget == owner)
		return 0;

	if (const ABaseEnemy* enemy = Cast<const ABaseEnemy>(&aimTarget))
	{
		if (ABimBallCharacter* player = Cast<ABimBallCharacter>(owner))
		{
			if ((!player->HasBall()) && (enemy->IsVulnrble() || player->IsEnraged()))
			{
				return USkill::GetAutoAimScore(aimTarget, aimData);
			}
		}
	}

	return 0;
}
