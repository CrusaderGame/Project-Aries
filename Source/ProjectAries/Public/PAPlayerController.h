// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PAPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTARIES_API APAPlayerController : public APlayerController
{
	GENERATED_BODY()

	public:
		APAPlayerController();
		void SetPlayerTeam(int32 TeamIndex);

		void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;




	private:

		UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Project Aries | Multi")
		int32 TeamIndex =-1;
};
