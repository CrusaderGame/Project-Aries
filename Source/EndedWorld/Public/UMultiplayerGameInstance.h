// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
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
		IOnlineSessionPtr OnlineSessionInterface;

	protected:
		UFUNCTION(BlueprintCallable, Category = "EW_Multi")
		void CreateGameSession();

		UFUNCTION(BlueprintCallable, Category = "EW_Multi")
		void JoinGameSession();

		void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
		void OnFindSessionsComplete(bool bWasSuccessful);
		void OnJoinSessionComplete(FName SessionName,EOnJoinSessionCompleteResult::Type Result);

	private:
		FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
		FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
		TSharedPtr<FOnlineSessionSearch> SessionSearch;
		FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
};
