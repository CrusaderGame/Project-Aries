// Fill out your copyright notice in the Description page of Project Settings.


#include "UMultiplayerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

UUMultiplayerGameInstance::UUMultiplayerGameInstance(const FObjectInitializer& ObjectInitializer)
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
