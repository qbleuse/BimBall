// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAnimation.h"

void UBaseEnemyAnimation::SetRun()
{
	runState = EEnemyRunState::RUN;
}

void UBaseEnemyAnimation::SetKZRun()
{
	runState = EEnemyRunState::KAMIKAZE_RUN;
}

void UBaseEnemyAnimation::SetTKRun()
{
	runState = EEnemyRunState::TACKLER_RUN;
}