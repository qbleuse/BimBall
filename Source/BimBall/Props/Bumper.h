// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Bumper.generated.h"

UCLASS()
class BIMBALL_API ABumper : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* mesh = nullptr;

	/* Impulse given to the overlapping entity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector impulse = FVector::UpVector * 100000.0;

public:	
	// Sets default values for this actor's properties
	ABumper();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* overlappedComp, 
						class AActor* otherActor,
						class UPrimitiveComponent* otherComp, 
						int32 otherBodyIndex, 
						bool bFromSweep, 
						const FHitResult& sweepResult);
};
