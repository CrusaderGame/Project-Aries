// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UMultiplayerGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ENDEDWORLD_API UUMultiplayerGameInstance : public UGameInstance
{
	GENERATED_BODY()


	public:
		UUMultiplayerGameInstance(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "EW_Multi")
		void OpenLobby();

	UFUNCTION(BlueprintCallable, Category = "EW_Multi")
		void CallOpenLevel(const FString& Address);

	UFUNCTION(BlueprintCallable, Category = "EW_Multi")
		void CallClientTravel(const FString& Address);

	public:
		// Pointer to the online session interface
		TSharedPtr< class IOnlineSession, ESPMode::ThreadSafe > OnlineSessionInterface;
};
