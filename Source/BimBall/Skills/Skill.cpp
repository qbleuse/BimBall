// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill.h"
#include "BimBall/Entity/Entity.h"

float USkill::GetAutoAimScore(const AActor& target, const FScoreAutoAim& autoAim) const
{
	verify(owner != nullptr);
	return GetAutoAimScore(*owner, target, autoAim);
}

float USkill::GetAutoAimScore(const AEntity& owner, const AActor& target, const FScoreAutoAim& autoAim)
{
	return USkill::GetAutoAimScore(owner, target, autoAim, owner.GetActorForwardVector());
}

float USkill::GetAutoAimScore(const AEntity& owner, const AActor& target, const FScoreAutoAim& autoAim, const FVector& direction)
{
	if (&target == &owner)
		return 0;

	if (owner.GetSquaredDistanceTo(&target) > FMath::Square(autoAim.maxDist))
	{
		return 0;
	}

	const float distScore = autoAim.distScoreScale / (target.GetDistanceTo(&owner) + 1.f);

	const float inFrontScore = autoAim.inFrontScoreScale * FVector::DotProduct(direction, (target.GetActorLocation() - owner.GetActorLocation()).GetSafeNormal());

	return distScore + inFrontScore;
}