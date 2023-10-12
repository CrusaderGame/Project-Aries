// Fill out your copyright notice in the Description page of Project Settings.

///OLD/////////////////////////////////////
#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"



ALobbyGameMode::ALobbyGameMode()
{

}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				60.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers)
			);

			APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
			if (PlayerState)
			{
				FString PlayerName = PlayerState->GetPlayerName();
				GEngine->AddOnScreenDebugMessage(
					-1,
					60.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s has joined the game!"), *PlayerName)
				);
			}

		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
	if (PlayerState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				60.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers - 1)
			);


			FString PlayerName = PlayerState->GetPlayerName();

			GEngine->AddOnScreenDebugMessage(
				-1,
				60.f,
				FColor::Cyan,
				FString::Printf(TEXT("%s has exited the game!"), *PlayerName)
			);
		}
	}

}

APawn* ALobbyGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	if (!StartSpot || !NewPlayer)
	{
		return nullptr;
	}


	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = NewPlayer;
	SpawnParams.Instigator = nullptr;
	SpawnParams.bDeferConstruction = false;

	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);

	if (!HasAuthority())
	{
		return nullptr;
	}

	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PawnClass, StartSpot->GetActorLocation(), StartSpot->GetActorRotation(), SpawnParams);
	if (!NewPawn)
	{
		return nullptr;
	}


	if (NewPawn)
	{
		NewPawn->SetReplicates(true);
		NewPawn->SetReplicateMovement(true);
	}

	return NewPawn;
}

AActor* ALobbyGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> UnOccupiedStarts;
	TArray<APlayerStart*> AllStarts;
	AActor* BestStart = nullptr;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		AllStarts.Add(*It);
	}

	for (APlayerStart* StartSpot : AllStarts)
	{
		if (IsPlayerStartSpotAvailable(StartSpot, Player))
		{
			UnOccupiedStarts.Add(StartSpot);
		}
	}

	if (UnOccupiedStarts.Num() > 0)
	{
		BestStart = UnOccupiedStarts[FMath::RandRange(0, UnOccupiedStarts.Num() - 1)];
	}
	else
	{
		BestStart = AllStarts[FMath::RandRange(0, AllStarts.Num() - 1)];
	}

	return BestStart;
}

bool ALobbyGameMode::IsPlayerStartSpotAvailable(APlayerStart* StartSpot, AController* Player)
{
	//TODO
	return true;
}