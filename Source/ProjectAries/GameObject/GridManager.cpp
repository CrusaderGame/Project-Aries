// 

#include "GridManager.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectAries/Game/BCGameState.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), FoundActors);
	if (FoundActors.Num() > 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Multiple actors of singleton class GridManager"));
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 999.0f, FColor::Red,
			TEXT("Multiple actors of singleton class GridManager")); }
	}
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

