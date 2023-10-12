// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EW_DefaultGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ENDEDWORLD_API AEW_DefaultGameMode : public AGameMode
{
	GENERATED_BODY()


public:
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
};
