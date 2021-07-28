// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleDummy.h"
#include "BimBall/Props/Ball/Ball.h"

void ADestructibleDummy::OnBallHit_Implementation(ABall* hitBall)
{
	Super::OnBallHit_Implementation(hitBall);

	this->DealDamages(hitBall->GetDamages());
}

void ADestructibleDummy::DealDamages(float rawDamages)
{
	life -= rawDamages;

	if (life <= 0)
	{
		this->Destroy();
	}
}