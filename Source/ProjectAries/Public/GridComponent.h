

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridComponent.generated.h"

USTRUCT(BlueprintType)
struct FGridLocation
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere)
	int32 Index;
	
	UPROPERTY(EditAnywhere)
	FVector Vector;
};

UENUM(BlueprintType) 
enum class EHeight : uint8 {
	OneLevel UMETA(DisplayName = "One Level"),
	Multilevel UMETA(DisplayName = "Multi Level"),
	EFalse UMETA(DisplayName = "EFalse"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTARIES_API UGridComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGridComponent();
	
	UPROPERTY(EditAnywhere)
	int32 IndexX = 1000;
	
	UPROPERTY(EditAnywhere)
	float HeightBetweenLevels = 200.0f;

	UPROPERTY(EditAnywhere)
	float MaxGridHeight = 200.0f;

	UPROPERTY(EditAnywhere)
	float MinGridHeight = -5.0f;
	
	UPROPERTY(EditAnywhere)
	int32 IndexZ = 1000000;

	UPROPERTY(EditAnywhere)
	int32 TileXSize = 200;
	
	UPROPERTY(EditAnywhere)
	int32 TileYSize = 200;
	
	UPROPERTY(EditAnywhere)
	TArray<FGridLocation> GridLocations;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	bool bSucces = false;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	bool CreateGridLocation(int32 StartIndex, int32 NumTilesX, int32 NumTilesY,EHeight HeightMap);
	
	UFUNCTION()
	FVector ConvertInexToLocationNaive(int32 GridIndex);
	
	std::vector<int>GetAllGridIndexesNaive( int32 NumTilesX, int32 NumTilesY,int32 StartIndex);
};
