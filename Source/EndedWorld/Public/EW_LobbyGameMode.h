// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EW_LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ENDEDWORLD_API AEW_LobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	

public:
	AEW_LobbyGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;


private:
	bool IsPlayerStartSpotAvailable(APlayerStart* StartSpot, AController* Player);


};
