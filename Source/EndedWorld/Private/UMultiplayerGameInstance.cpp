// Fill out your copyright notice in the Description page of Project Settings.


#include "UMultiplayerGameInstance.h"
#include "Kismet/GameplayStatics.h"

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
/*


void AMyCharacterTEST::CallOpenLevel(const FString& Address)
{
	UGameplayStatics::OpenLevel(this, *Address);
}

void AMyCharacterTEST::CallClientTravel(const FString& Address)
{
	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}

*/