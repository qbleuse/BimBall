// Fill out your copyright notice in the Description page of Project Settings.


#include "RailManager.h"
#include "Engine/Engine.h"

// Sets default values
ARailManager::ARailManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARailManager::BeginPlay()
{
	Super::BeginPlay();

	ComputeRailsTotalLength();

	for (int32 i = 0; i < railedActorsData.Num(); i++)
	{
		if (railedActorsData[i].railedActor != nullptr)
		{
			railedActorsData[i].firstLocation = railedActorsData[i].railedActor->GetActorLocation();
		}
	}
}

void ARailManager::ComputeRailsTotalLength()
{
	totalRailsLength = 0;
	for (int32 i = 0; i < rails.Num(); i++)
	{
		totalRailsLength += FVector::Dist(rails[i]->GetActorLocation(),
										  rails[(i + 1) % rails.Num()]->GetActorLocation());
	}
}

// Called every frame
void ARailManager::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	UpdateRailedActorsLocation(deltaTime);
}

void ARailManager::UpdateRailedActorsLocation(float deltaTime)
{
	for (int32 i = 0; i < railedActorsData.Num(); i++)
	{
		FRailedActorData& railedActorData = railedActorsData[i];
		AActor* comp = railedActorData.railedActor;

		if (comp == nullptr || railedActorData.currentRailIndex >= rails.Num())
		{
			continue;
		}

		const FVector compLocation = comp->GetActorLocation();
		const FVector nextRailLocation = rails[railedActorData.currentRailIndex]->GetActorLocation();

		FVector compToNextRail = nextRailLocation - compLocation;
		FVector newLoc = compLocation + compToNextRail.GetSafeNormal() * speed * deltaTime;

		if (FVector::DistSquared(compLocation, newLoc) >= FVector::DistSquared(compLocation, nextRailLocation))
		{
			/* diff > 0  */
			const float diff = speed - FVector::Dist(compLocation, nextRailLocation);

			comp->SetActorLocation(nextRailLocation, true);

			railedActorData.currentRailIndex++;
			if (state == ERailBehavior::LOOPING && railedActorData.currentRailIndex >= rails.Num())
			{
				railedActorData.currentRailIndex = 0;
			}

			const FVector nextRailLocation2 = rails[railedActorData.currentRailIndex]->GetActorLocation();
			FVector compToNextRail2 = nextRailLocation2 - nextRailLocation;
			FVector newLoc2 = nextRailLocation + compToNextRail2.GetSafeNormal() * diff * deltaTime;
			comp->SetActorLocation(newLoc2, true);
		}
		else
		{
			comp->SetActorLocation(newLoc, true);
		}


		/* Set Correct Location */
		/* Imprecision caused by float imprecision, so at regular intervals, we set the location of the actor again. */
		const float timeToTravelAllRails = totalRailsLength / speed;
		if (GetWorld()->GetTimeSeconds() > railedActorData.lastLocCorrectionTime + timeToTravelAllRails)
		{
			railedActorData.lastLocCorrectionTime += timeToTravelAllRails;
			railedActorData.railedActor->SetActorLocation(railedActorData.firstLocation);
		}
	}
}