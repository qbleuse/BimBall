// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BimBallGameState.generated.h"

/**
 * 
 */
UCLASS()
class BIMBALL_API ABimBallGameState : public AGameState
{
	GENERATED_BODY()
	
protected:
	uint32 score = 0;
};
