// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LevelStreamingLibrary.generated.h"

/**
 * 
 */
UCLASS()
class BIMBALL_API ULevelStreamingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void LoadStreamLevel();
};
