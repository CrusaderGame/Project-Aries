// Fill out your copyright notice in the Description page of Project Settings.


#include "UMultiplayerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UUMultiplayerGameInstance::UUMultiplayerGameInstance(const FObjectInitializer& ObjectInitializer):
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnCreateSessionComplete))
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Found subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString()));

		}
	}
}

void UUMultiplayerGameInstance::OpenLobby()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel("/Game/Levels/Sublevels/dam2452-Test?listen");
	}
}

void UUMultiplayerGameInstance::CallOpenLevel(const FString& Address)
{
	UGameplayStatics::OpenLevel(this, *Address);
}

void UUMultiplayerGameInstance::CallClientTravel(const FString& Address)
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}

void UUMultiplayerGameInstance::CreateGameSession()
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = false;
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;


	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	//GPT4 LocalPlayer Debug
	/*if (LocalPlayer)
	{
		FUniqueNetIdRepl PlayerIDRepl = LocalPlayer->GetPreferredUniqueNetId();
		if (PlayerIDRepl.IsValid())
		{
			TSharedPtr<const FUniqueNetId> PlayerID = PlayerIDRepl.GetUniqueNetId();
			if (PlayerID.IsValid())
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString::Printf(TEXT("Player ID: %s"), *PlayerID->ToString()));
				OnlineSessionInterface->CreateSession(*PlayerID, NAME_GameSession, *SessionSettings);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Player ID TSharedPtr is not valid!"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Player IDRepl is not valid!"));
		}
	}*/
	//


	if (OnlineSessionInterface.IsValid() && SessionSettings.IsValid())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, TEXT("Preparing to create session..."));
		}
	}

	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);

}

void UUMultiplayerGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Create session: %s"), *SessionName.ToString()));
		}
	}

	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Faild to create session!")));
		}
	}

}
