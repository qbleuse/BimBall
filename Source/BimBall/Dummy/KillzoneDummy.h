// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BimBall/Dummy/BaseDummy.h"
#include "KillzoneDummy.generated.h"

/**
 * Dummy that can stagger and cause damages when an entity collides it.
 */
UCLASS()
class BIMBALL_API AKillzoneDummy : public ABaseDummy
{
	GENERATED_BODY()
	
	/* Staggers and deals damages on hit (when up) */
	virtual void OnHit(UPrimitiveComponent* hitComponent, 
					   AActor* otherActor, 
					   UPrimitiveComponent* otherComponent, 
					   FVector normalImpulse, 
					   const FHitResult& hit) override;
};
