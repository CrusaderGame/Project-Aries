// Temp

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATBTTGridManager.generated.h"

//Structs

USTRUCT(BlueprintType)
struct FEdgesCpp
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		TMap<int32, int32> Edges;
};

USTRUCT(BlueprintType)
struct FPathfindingCpp
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
		int32 Cost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
		int32 Parent;
};

USTRUCT(BlueprintType)
struct FPathListItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
		int32 Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
		int32 Cost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
		int32 Parent;
};

USTRUCT(BlueprintType)
struct FNestedInts
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		TArray<int32> Integers;
};

//Enums

UENUM()
enum EHeightCpp
{
	Disabled     UMETA(DisplayName = "Disabled"),
	OneLevel      UMETA(DisplayName = "OneLevel"),
	Multilevel   UMETA(DisplayName = "Multilevel"),
};

UCLASS()
class AATBTTGridManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AATBTTGridManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		int32 GridSizeXCpp = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		int32 GridSizeYCpp = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Grid)
		int32 GridSizeZCpp = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		int32 IndexXCpp = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		int32 IndexZCpp = 1000000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		float TileSizeCpp = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		float MaxGridHeightCpp = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		float MinGridHeightCpp = -100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		float HeightBetweenLevelsCpp = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		bool bTraceForWallsCpp = true;

	//Procedural

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Procedural)
		float HeightImpassableCutoffCpp = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Procedural)
		float HeightSlowIncrementCpp = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Procedural)
		TEnumAsByte<EHeightCpp> HeightmapCpp = Disabled;

	// Arrays and Maps
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		TMap<int32, FVector> GridLocationsCpp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		TArray<int32> BaseEdgesCpp{-1,-1000,1,1000};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		TMap<int32, FEdgesCpp> GridEdgesCpp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		TMap<int32, FNestedInts> HeightMapLevelsCpp;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Grid")
		void CreateGridLocationsCpp(TMap<int32, FVector>& GridLocations, EHeightCpp Heightmap);

	UFUNCTION(BlueprintCallable, Category = "Grid")
		void SetEdgesBasedOnTerrainCpp(bool bTraceForWalls, EHeightCpp Heightmap, TMap<int32, FEdgesCpp>& GridEdges);

	UFUNCTION(BlueprintCallable, Category = "Grid")
		int32 GetEdgeCostFromZDifferenceCpp(float ParentZ, float ChildZ, float ImpassableCutoff, float CostIncrement);

	UFUNCTION(BlueprintCallable, Category = "Grid")
		bool TraceOnGridCpp(int32 StartIndex, int32 TargetIndex, float TraceHeight);

	UFUNCTION(BlueprintCallable, Category = "Visibility")
		TArray<int32> GetIndexesInRangeCpp(int32 StartIndex, int32 Range, bool bDiamondShaped);

	UFUNCTION(BlueprintCallable, Category = "Visibility")
		TSet<int32> FindVisibleTilesFromTilesInRangeCpp(int32 StartIndex, TArray<int32> InRangeTiles, float MaxZDifference, int32 MinRange, bool bCheckVisibility, float TraceHeight, bool bDiamondShaped);

	UFUNCTION(BlueprintCallable, Category = "Visibility")
		bool CheckIfTileIsVisibleFromOtherTileCpp(int32 StartIndex, int32 TargetIndex, float MaxZDifference = 999999.0f, int32 MinRange = 0, bool bCheckVisibility = true, float TraceHeight = 170.0f, bool bDiamondShaped = false);

	UFUNCTION(BlueprintCallable, Category = "Visibility")
		TSet<int32> FindTilesInRangeCpp(int32 StartIndex, int32 Range, float MaxZDifference = 999999.0f, int32 MinRange = 0, bool bCheckVisibility = true, float TraceHeight = 170.0f, bool bDiamondShaped = false);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		void RunPathfindingCpp(int32 StartIndex, int32 MoveRange, TMap<int32,FPathfindingCpp>& PathsMap);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		bool CheckIfImpassableCpp(int32 GridIndex);

	UFUNCTION(BlueprintCallable, Category = "Grid")
		int32 ConvertLocaionToIndexCpp(FVector Location) const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
		void UpdateHeightmapCacheCpp(int32 GridIndex);

	UFUNCTION(BlueprintCallable, Category = "Grid")
		TArray<FVector> ConvertGridIndexesToLocationsCpp(TArray<int32> GridIndexes, FVector Offset);

	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		TArray<int32> FindPathToIndexCpp(TMap<int32,FPathfindingCpp> PathsMap, int32 EndIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
