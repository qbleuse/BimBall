// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RailManager.generated.h"

UENUM()
enum class ERailBehavior : uint8
{
	ONEWAY     UMETA(DisplayName = "OneWay"),
	//REVERSING  UMETA(DisplayName = "Reversing"),  // To Implement
	LOOPING    UMETA(DisplayName = "Looping"),
};

USTRUCT(BlueprintType)
struct FRailedActorData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* railedActor = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 currentRailIndex = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float lastLocCorrectionTime = 0.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector firstLocation = FVector::ZeroVector;
};

UCLASS()
class BIMBALL_API ARailManager : public AActor
{
	GENERATED_BODY()
	
protected:
	/* Array of the railed actor and their data. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FRailedActorData> railedActorsData;

	/* Array of rail points. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AActor*> rails;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ERailBehavior state = ERailBehavior::LOOPING;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float speed = 300;

	/* Length between each rail. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float totalRailsLength = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Used to fix float imprecision errors. */
	void UpdateRailedActorsLocation(float deltaTime);

public:	
	/* Sets default values for this actor's properties */
	ARailManager();

	/* Called every frame */
	virtual void Tick(float deltaTime) override;

	/* Compute the length between each rail. */
	void ComputeRailsTotalLength();

};
