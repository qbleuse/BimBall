// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Yard.generated.h"


/* Touchdown zone */
UCLASS()
class BIMBALL_API AYard : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* yardZone = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBillboardComponent* enemyTouchdownPoint = nullptr;

public:


public:	
	// Sets default values for this actor's properties
	AYard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnYardOverlapBegin(class UPrimitiveComponent* OverlappedComponent,
							class AActor* OtherActor,
							class UPrimitiveComponent* OtherComp,
							int32 OtherBodyIndex,
							bool bFromSweep,
							const FHitResult& SweepResult);


	UFUNCTION(BlueprintNativeEvent)
	void OnPlayerEnterYard(class ABimBallCharacter* player);
	void OnPlayerEnterYard_Implementation(class ABimBallCharacter* player);

	UFUNCTION(BlueprintNativeEvent)
	void RunEnemyTouchdown(class ABaseEnemy* enemy);
	void RunEnemyTouchdown_Implementation(class ABaseEnemy* enemy);

	UFUNCTION(BlueprintCallable)
	FVector GetEnemyTouchdownLocation() const;
};
