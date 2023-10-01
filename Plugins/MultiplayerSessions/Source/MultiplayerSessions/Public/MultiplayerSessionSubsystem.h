// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionSubsystem.generated.h"

//
// Daclaring custom delegates
//

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);



//USTRUCT(BlueprintType)
//struct FBlueprintSessionResultWrapper
//{
//	GENERATED_BODY()
//
//	FOnlineSessionSearchResult InternalResult;
//};

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMultiplayerSessionSubsystem();

	//
	// To handle session functionality
	//
	//UFUNCTION(BlueprintCallable, Category = "MultiplayerSessionSubsystem")
	void CreateSession(int32 NumPublicConnections, FString MatchType);

	//UFUNCTION(BlueprintCallable, Category = "MultiplayerSessionSubsystem")
	void FindSession(int32 MaxSearchResults);

	//UFUNCTION(BlueprintCallable, Category = "MultiplayerSessionSubsystem")
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);

	//UFUNCTION(BlueprintCallable, Category = "MultiplayerSessionSubsystem")
	void DestroySession();

	//UFUNCTION(BlueprintCallable, Category = "MultiplayerSessionSubsystem")
	void StartSession();

	//
	// Custom callback
	//

	//UPROPERTY(BlueprintAssignable, Category = "MultiplayerSessionSubsystem")
	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;

	//
	//BP Implementation
	//


protected:
	//
	// Internal callbacks for the delegates 
	// Thise dont't need to be called outside this class
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

	//
	// Callbacks for the custom delegates 
	//
	void OnCreateSession(bool bWasSuccessful);
	
	

private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;

	//
	// To add to the Online Session Interface delegate list.
	//
	
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate StartSessionCompleteDelegat;
	FDelegateHandle StartSessionCompleteDelegatHandle;
};
