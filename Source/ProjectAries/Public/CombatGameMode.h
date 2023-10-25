// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CombatGameMode.generated.h"

/**
 *
 */
UCLASS()
class PROJECTARIES_API ACombatGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACombatGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	UFUNCTION(BlueprintCallable, Category = "Project Aries|Players")
	APlayerController* GetHostPlayer() const;

	UFUNCTION(BlueprintCallable, Category = "Project Aries|Players")
	APlayerController* GetSecondPlayer() const;

	UFUNCTION(BlueprintCallable, Category = "Project Aries|Players")
	APlayerController* GetThirdPlayer() const;

	UFUNCTION(BlueprintCallable, Category = "Project Aries|Players")
	APlayerController* GetFourthPlayer() const;

private:
	APlayerController* HostPlayer = nullptr;
	APlayerController* SecondPlayer = nullptr;
	APlayerController* ThirdPlayer = nullptr;
	APlayerController* FourthPlayer = nullptr;
};

