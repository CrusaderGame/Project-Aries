// Fill out your copyright notice in the Description page of Project Settings.


#include "EW_DefaultGameMode.h"

APawn* AEW_DefaultGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
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


    APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PawnClass, StartSpot->GetActorLocation(), StartSpot->GetActorRotation(), SpawnParams);
    if (!NewPawn)
    {
        return nullptr;
    }

    return NewPawn;
}
