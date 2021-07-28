// Fill out your copyright notice in the Description page of Project Settings.


#include "BimBallCharacterAnimation.h"
#include "BimBall/Props/Ball/Ball.h"
#include "BimBall/Entity/Player/BimBallCharacter.h"

void UBimBallCharacterAnimation::NativeBeginPlay()
{
	UAnimInstance::NativeBeginPlay();
	pawnOwner = Cast<ABimBallCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void UBimBallCharacterAnimation::UpdateBallRelativeLoc(const ABall* ball)
{
	isBallHigherThanPlayer = ball->GetActorLocation().Z >= pawnOwner->GetActorLocation().Z;
}

void UBimBallCharacterAnimation::RunAimAnim()
{
	animState = EBimBallAnimState::AIM;
}

void UBimBallCharacterAnimation::RunThrowAnim()
{
	animState = EBimBallAnimState::THROW;
}

void UBimBallCharacterAnimation::RunGrabAnim()
{
	animState = EBimBallAnimState::GRABBING;
}

void UBimBallCharacterAnimation::RunFlingAnim()
{
	animState = EBimBallAnimState::IDLE;
}