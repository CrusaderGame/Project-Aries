// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatGameMode.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include <PAPlayerController.h>

ACombatGameMode::ACombatGameMode()
{
}

void ACombatGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (HasAuthority())
    {
    UE_LOG(LogTemp, Warning, TEXT("Player joined: %s"), *NewPlayer->GetName());

    APAPlayerController* PAPlayerControler = Cast<APAPlayerController>(NewPlayer);

    if (PAPlayerControler)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player is using APAPlayerController: %s"), *PAPlayerControler->GetName());

        if (!HostPlayer)
        {
            HostPlayer = NewPlayer;
            PAPlayerControler->SetPlayerTeam(0);
            UE_LOG(LogTemp, Warning, TEXT("Assigned team 0 to player: %s"), *HostPlayer->GetName());
        }
        else if (!SecondPlayer)
        {
            SecondPlayer = NewPlayer;
            PAPlayerControler->SetPlayerTeam(1);
            UE_LOG(LogTemp, Warning, TEXT("Assigned team 1 to player: %s"), *SecondPlayer->GetName());
        }
        else if (!ThirdPlayer)
        {
            ThirdPlayer = NewPlayer;
            PAPlayerControler->SetPlayerTeam(2);
            UE_LOG(LogTemp, Warning, TEXT("Assigned team 2 to player: %s"), *ThirdPlayer->GetName());
        }
        else if (!FourthPlayer)
        {
            FourthPlayer = NewPlayer;
            PAPlayerControler->SetPlayerTeam(3);
            UE_LOG(LogTemp, Warning, TEXT("Assigned team 3 to player: %s"), *FourthPlayer->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Player is NOT using APAPlayerController: %s"), *NewPlayer->GetName());
    }
}
}

void ACombatGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APAPlayerController* PAPlayerControler = Cast<APAPlayerController>(Exiting);

    if (PAPlayerControler)
    {
        if (HostPlayer == Exiting)
        {
			HostPlayer = nullptr;
		}
        else if (SecondPlayer == Exiting)
        {
			SecondPlayer = nullptr;
		}
        else if (ThirdPlayer == Exiting)
        {
			ThirdPlayer = nullptr;
		}
        else if (FourthPlayer == Exiting)
        {
			FourthPlayer = nullptr;
		}
	}
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
