// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurnManager.generated.h"

UCLASS()
class PROJECTARIES_API ATurnManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurnManager();
	void Initialize();
	void NotifyPlayerReady(APlayerController* Player);
	void StartMatch();
	void BeginNewRound();
	void BeginNextValidActorTurn();
	void UnitSelected(AActor* Actor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
