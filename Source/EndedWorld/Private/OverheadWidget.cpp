// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
//ONLY FOR LAN CHECK
#include "OnlineSubsystem.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
    FString PlayerName = "localPlayer";

    if (InPawn->HasAuthority())
    {
        if (InPawn && InPawn->GetPlayerState())
        {
            APlayerState* PlayerState = Cast<APlayerState>(InPawn->GetPlayerState());
            if (PlayerState)
            {
                PlayerName = PlayerState->GetPlayerName();
            }
        }
    }

   

    ENetRole LocalRole = InPawn->GetLocalRole();
    FString Role;
    switch (LocalRole)
    {
    case ENetRole::ROLE_Authority:
        Role = FString("Authority");
        break;
    case ENetRole::ROLE_AutonomousProxy:
        Role = FString("Autonomous Proxy");
        break;
    case ENetRole::ROLE_SimulatedProxy:
        Role = FString("Simulated Proxy");
        break;
    case ENetRole::ROLE_None:
        Role = FString("None");
        break;
    }

    FString LocalRoleString = FString::Printf(TEXT("%s\nLocal Role: %s"), *PlayerName, *Role);

    SetDisplayText(LocalRoleString);
}


void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();

}
