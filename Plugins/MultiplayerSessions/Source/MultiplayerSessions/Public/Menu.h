// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MultiplayerSessionSubsystem.h"
#include "Components/Button.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "MultiplayerMenu")
	void MenuSetup();

protected:
	virtual bool Initialize() override;

private:

	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	//Subsystem onilne session
	UMultiplayerSessionSubsystem* MultiplayerSessionSubsystem;

};
