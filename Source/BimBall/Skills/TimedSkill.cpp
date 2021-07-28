// Fill out your copyright notice in the Description page of Project Settings.


#include "TimedSkill.h"
#include "BimBall/Entity/Entity.h"

void UTimedSkill::Start_Implementation()
{
	verify(owner != nullptr);
	startTime = owner->GetWorld()->GetTimeSeconds();
}


void UTimedSkill::Tick_Implementation(float deltaTime)
{
	verify(owner != nullptr);
	/* If enough time has passed since the start of the game, ends  */
	if (startTime + duration < owner->GetWorld()->GetTimeSeconds())
	{
		End();
	}
}