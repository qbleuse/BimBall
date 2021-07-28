// Fill out your copyright notice in the Description page of Project Settings.


#include "BimBall/Dummy/AnimDummy.h"

void UAnimDummy::OnUp()
{
	isUp = true;
}

void UAnimDummy::OnDown(bool downOnPositiveSide)
{
	isUp = false;

	downRotation = downOnPositiveSide ? 90 : -90;
}