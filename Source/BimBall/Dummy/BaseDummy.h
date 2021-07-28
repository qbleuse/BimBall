// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "BimBall/BallHittable.h"
#include "BimBall/Skills/StaggerData.h"
#include "BaseDummy.generated.h"

/*
 * A dummy is an object that can be bend when hit.
 * If up, it blocks the player.
 */
UCLASS()
class BIMBALL_API ABaseDummy : public AActor, public IBallHittable
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* dummyMesh = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* boxCollider = nullptr;

	/* Socket corresponding to the bone at which the dummy should bend. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName colliderAttachedSocket = "Bone_001";

	/* The delay after which the dummy lifts up again. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float liftUpDelay = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FStaggerData hitStaggerData;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float damagesOnHit = 0.f;

	FTimerHandle liftUpHandle;
	bool isUp = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool isLiftingUp = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ABaseDummy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StretchOut(bool downOnPositiveSide);

	/* Stretch out in the opposite direction of the actor. */
	UFUNCTION(BlueprintCallable)
	void StretchOutRelativeToActor(AActor const * const actor);

	/* Lifts up the Dummy, giving it back its collisions and staggering the pl */
	UFUNCTION(BlueprintCallable)
	void LiftUp();

	/* When the ball hits this actor */
	virtual void OnBallHit_Implementation(class ABall* hitBall) override;

	/* When this actor hits another component */
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector normalImpulse, const FHitResult& hit);

	UFUNCTION()
	FORCEINLINE bool IsUp() const
	{
		return isUp;
	}
};
