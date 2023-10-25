// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGameMode.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"

ACombatGameMode::ACombatGameMode()
{
}

void ACombatGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

		if (!HostPlayer)
		{
			HostPlayer = NewPlayer;
		}
		else if (!SecondPlayer)
		{
			SecondPlayer = NewPlayer;
		}
		else if (!ThirdPlayer)
		{
			ThirdPlayer = NewPlayer;
		}
		else if (!FourthPlayer)
		{
			FourthPlayer = NewPlayer;
		}



	}
}

void ACombatGameMode::Logout(AController* Exiting)
{
}

APlayerController* ACombatGameMode::GetHostPlayer() const
{

	return HostPlayer;
}

APlayerController* ACombatGameMode::GetSecondPlayer() const
{
	return SecondPlayer;
}

APlayerController* ACombatGameMode::GetThirdPlayer() const
{
	return ThirdPlayer;
}

APlayerController* ACombatGameMode::GetFourthPlayer() const
{
	return FourthPlayer;
}
