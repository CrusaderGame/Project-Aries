// Fill out your copyright notice in the Description page of Project Settings.


#include "PAPlayerController.h"
#include <Net/UnrealNetwork.h>

APAPlayerController::APAPlayerController()
{
	bReplicates = true;
}

void APAPlayerController::SetPlayerTeam(int32 Index)
{
	TeamIndex = Index;
}

void APAPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APAPlayerController, TeamIndex);
}
