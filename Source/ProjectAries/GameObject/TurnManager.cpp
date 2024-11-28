// 

#include "TurnManager.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectAries/Game/BCGameState.h"

ATurnManager::ATurnManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATurnManager::Initialize()
{
	//SortActorsInInitiativeOrder();
}

void ATurnManager::NotifyPlayerReady(APlayerController* Player)
{
	//AssignUnitsToPlayers();
	//ReplacePreviewPuppetsWithFinal();
}

void ATurnManager::StartMatch()
{
	//GetAllPlayerControllers()
	//ActivateAbility on them
	//SetTurnState TurnBased
	auto GameState = Cast<ABCGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (IsValid(GameState)) { GameState->SetTurnState(Running); }
	
	// BeginNewRound();
}

void ATurnManager::BeginNewRound()
{
}

void ATurnManager::BeginNextValidActorTurn()
{
}

void ATurnManager::UnitSelected(AActor* Actor)
{
}

void ATurnManager::BeginPlay()
{
	Super::BeginPlay();

	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurnManager::StaticClass(), FoundActors);
	if (FoundActors.Num() > 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Multiple actors of singleton class TurnManager"));
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 999.0f, FColor::Red,
			TEXT("Multiple actors of singleton class TurnManager")); }
	}
}

void ATurnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

