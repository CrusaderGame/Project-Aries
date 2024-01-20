
#include "ATBTTGridManager.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "..\Public\ATBTTGridManager.h"

#define PathTrace ECC_GameTraceChannel1

// Sets default values
AATBTTGridManager::AATBTTGridManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AATBTTGridManager::CreateGridLocationsCpp(TMap<int32, FVector>& GridLocations, EHeightCpp Heightmap)
{
	GridLocationsCpp.Reset();

	switch (Heightmap)
	{
	case Disabled:
	{
		for (int32 i = 0; i < ((GridSizeXCpp*GridSizeYCpp)); i++)
		{
			GridLocationsCpp.Add((((i%GridSizeXCpp)*IndexXCpp) + (i / GridSizeXCpp)), FVector((i%GridSizeXCpp)*TileSizeCpp, (i / GridSizeXCpp)*TileSizeCpp, 0.0f) + GetActorLocation());
		}

		GridLocations = GridLocationsCpp;

		return;
	}

	case OneLevel:
	{
		FHitResult Hit;

		FCollisionQueryParams TraceParams;

		FVector Start;

		FVector End;

		for (int32 i = 0; i < ((GridSizeXCpp*GridSizeYCpp)); i++)
		{
			Start = (GetTransform().TransformPosition(FVector((i%GridSizeXCpp)*TileSizeCpp, (i / GridSizeXCpp)*TileSizeCpp, MaxGridHeightCpp)));
			End = (GetTransform().TransformPosition(FVector((i%GridSizeXCpp)*TileSizeCpp, (i / GridSizeXCpp)*TileSizeCpp, MinGridHeightCpp)));

			GetWorld()->LineTraceSingleByChannel(Hit, Start, End, PathTrace, TraceParams);

			DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 2.0f);

			if (Hit.bBlockingHit)
			{
				DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);
				GridLocationsCpp.Add((((i%GridSizeXCpp)*IndexXCpp) + (i / GridSizeXCpp)), GetTransform().InverseTransformPosition(Hit.ImpactPoint));
			}

		}

		GridLocations = GridLocationsCpp;

		return;
	}

	case Multilevel:
	{

		HeightMapLevelsCpp.Empty();

		FHitResult Hit;

		FCollisionQueryParams TraceParams;

		FVector Start;

		FVector End;

		for (int32 i = 0; i < ((GridSizeXCpp*GridSizeYCpp)); i++)
		{
			Start = (GetTransform().TransformPosition(FVector((i%GridSizeXCpp)*TileSizeCpp, (i / GridSizeXCpp)*TileSizeCpp, MaxGridHeightCpp)));

			FVector LastHit{ 0.0f, 0.0f, -9999999.0f };

			do
			{

				End = (GetTransform().TransformPosition(FVector((i%GridSizeXCpp)*TileSizeCpp, (i / GridSizeXCpp)*TileSizeCpp, MinGridHeightCpp - 1.0f)));

				GetWorld()->LineTraceSingleByChannel(Hit, Start, End, PathTrace, TraceParams);

				if (Hit.bBlockingHit)
				{
					if (abs((LastHit - Hit.Location).Z) >= HeightBetweenLevelsCpp)
					{
						LastHit = Hit.Location;

						Start = (LastHit + (GetActorUpVector() * GetActorScale3D()));
						End = (LastHit + ((GetActorUpVector() * GetActorScale3D())*(HeightBetweenLevelsCpp)));

						GetWorld()->LineTraceSingleByChannel(Hit, Start, End, PathTrace, TraceParams);


						if (!Hit.bBlockingHit)
						{

							int32 HitGridIndex = ConvertLocaionToIndexCpp(LastHit);

							GridLocationsCpp.Add(HitGridIndex, GetTransform().InverseTransformPosition(LastHit));

							UpdateHeightmapCacheCpp(HitGridIndex);

							GridSizeZCpp = FMath::Max((HitGridIndex / IndexZCpp + 1), GridSizeZCpp);

						}


					}
				}
				else
				{
					break;
				}

				Start.Z = LastHit.Z - HeightBetweenLevelsCpp;


			} while ((LastHit.Z >= (MinGridHeightCpp + HeightBetweenLevelsCpp)));

		}


	}

	GridLocations = GridLocationsCpp;

	return;

	}

}


void AATBTTGridManager::SetEdgesBasedOnTerrainCpp(bool bTraceForWalls, EHeightCpp Heightmap, TMap<int32, FEdgesCpp>& GridEdges)
{
	switch (Heightmap)
	{
	case Disabled:

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Disabled"));
		for (auto& Elem : GridLocationsCpp)
		{
			FEdgesCpp LocalEdges;
			for (int32 BaseEdge : BaseEdgesCpp)
			{
				if (GridLocationsCpp.Contains(Elem.Key + BaseEdge))
				{
					if (bTraceForWalls)
					{
						if (!TraceOnGridCpp(Elem.Key, Elem.Key + BaseEdge, 100.0f))
						{
							LocalEdges.Edges.Add(Elem.Key + BaseEdge, 1);
						}
					}
					else
					{
						LocalEdges.Edges.Add(Elem.Key + BaseEdge, 1);
					}
				}
			};
			GridEdgesCpp.Add(Elem.Key, LocalEdges);
		};
		GridEdges = GridEdgesCpp;
		return;

	case OneLevel:

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("OneLevel"));
		for (auto& Elem : GridLocationsCpp)
		{
			FEdgesCpp LocalEdges;
			for (int32 BaseEdge : BaseEdgesCpp)
			{
				if (GridLocationsCpp.Contains(Elem.Key + BaseEdge))
				{
					int32 EdgeCost = GetEdgeCostFromZDifferenceCpp(GridLocationsCpp[Elem.Key].Z, GridLocationsCpp[Elem.Key + BaseEdge].Z, HeightImpassableCutoffCpp, HeightSlowIncrementCpp);
					if (EdgeCost > 0)
					{
						if (bTraceForWalls)
						{
							if (!TraceOnGridCpp(Elem.Key, Elem.Key + BaseEdge, 100.0f))
							{
								LocalEdges.Edges.Add(Elem.Key + BaseEdge, EdgeCost);
							}
						}
						else
						{
							LocalEdges.Edges.Add(Elem.Key + BaseEdge, EdgeCost);
						}
					}
				}
			};
			GridEdgesCpp.Add(Elem.Key, LocalEdges);
		};
		GridEdges = GridEdgesCpp;
		return;

	case Multilevel:

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Multilevel"));

		TMap<int32, int32> LocalEdges;

		int32 CurrentIndex;

		for (auto Elem : HeightMapLevelsCpp)
		{
			int32 GridIndex = Elem.Key;
			TArray<int32> Levels = Elem.Value.Integers;

			for (auto i = Levels.Num() - 1; i >= 0; i--)
			{
				LocalEdges.Reset();

				CurrentIndex = Levels[i] * IndexZCpp + GridIndex;

				if (GridEdgesCpp.Contains(CurrentIndex + IndexZCpp))
				{

					TMap<int32, int32> LocalAboveEdges = GridEdgesCpp[CurrentIndex + IndexZCpp].Edges;

					for (int32 BaseEdge : BaseEdgesCpp)
					{
						for (int32 relativeLevel = 1; relativeLevel >= -1; relativeLevel--)
						{
							int32 AdjacentTile = (relativeLevel * IndexZCpp) + (BaseEdge + CurrentIndex);

							if (GridLocationsCpp.Contains(AdjacentTile))
							{
								if (!LocalAboveEdges.Contains(AdjacentTile))

									if ((GetEdgeCostFromZDifferenceCpp(GridLocationsCpp[CurrentIndex].Z, GridLocationsCpp[AdjacentTile].Z, HeightImpassableCutoffCpp, HeightSlowIncrementCpp)) > 0)
									{
										if (bTraceForWalls)
										{
											if (!TraceOnGridCpp(CurrentIndex, AdjacentTile, 100.0f))
											{
												LocalEdges.Add(AdjacentTile, (GetEdgeCostFromZDifferenceCpp(GridLocationsCpp[CurrentIndex].Z, GridLocationsCpp[AdjacentTile].Z, HeightImpassableCutoffCpp, HeightSlowIncrementCpp)));
												break;
											}
										}
										else
										{
											LocalEdges.Add(AdjacentTile, (GetEdgeCostFromZDifferenceCpp(GridLocationsCpp[CurrentIndex].Z, GridLocationsCpp[AdjacentTile].Z, HeightImpassableCutoffCpp, HeightSlowIncrementCpp)));
											break;
										}
									}

							}

						}

					}

				}

				else
				{
					for (int32 BaseEdge : BaseEdgesCpp)
					{
						for (int32 relativeLevel = 1; relativeLevel >= -1; relativeLevel--)
						{
							int32 AdjacentTile = (relativeLevel * IndexZCpp) + (BaseEdge + CurrentIndex);

							if (GridLocationsCpp.Contains(AdjacentTile))
							{

								if ((GetEdgeCostFromZDifferenceCpp(GridLocationsCpp[CurrentIndex].Z, GridLocationsCpp[AdjacentTile].Z, HeightImpassableCutoffCpp, HeightSlowIncrementCpp)) > 0)
								{
									if (bTraceForWalls)
									{
										if (!TraceOnGridCpp(CurrentIndex, AdjacentTile, 100.0f))
										{
											LocalEdges.Add(AdjacentTile, (GetEdgeCostFromZDifferenceCpp(GridLocationsCpp[CurrentIndex].Z, GridLocationsCpp[AdjacentTile].Z, HeightImpassableCutoffCpp, HeightSlowIncrementCpp)));
											break;
										}
									}
									else
									{
										LocalEdges.Add(AdjacentTile, (GetEdgeCostFromZDifferenceCpp(GridLocationsCpp[CurrentIndex].Z, GridLocationsCpp[AdjacentTile].Z, HeightImpassableCutoffCpp, HeightSlowIncrementCpp)));
										break;
									}
								}

							}

						}

					}

				}

				GridEdgesCpp.Add(CurrentIndex, FEdgesCpp{ LocalEdges });

			}

		}

		GridEdges = GridEdgesCpp;
		return;

	}

	return;

};




int32 AATBTTGridManager::GetEdgeCostFromZDifferenceCpp(float ParentZ, float ChildZ, float ImpassableCutoff, float CostIncrement)
{
	if (abs(ParentZ - ChildZ) < CostIncrement)
	{
		return 1;
	}
	else if ((abs(ParentZ - ChildZ) < ImpassableCutoff))
	{
		return (floor(abs(ParentZ - ChildZ) / CostIncrement) + 1);
	}
	return 0;
}

bool AATBTTGridManager::TraceOnGridCpp(int32 StartIndex, int32 TargetIndex, float TraceHeight)
{
	FHitResult Hit;

	FCollisionQueryParams TraceParams;

	FVector Start;

	FVector End;

	Start = GridLocationsCpp[StartIndex] + FVector(0.0f, 0.0f, TraceHeight);

	End = GridLocationsCpp[TargetIndex] + FVector(0.0f, 0.0f, TraceHeight);

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel4, TraceParams);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 5.0f);

	return Hit.bBlockingHit;
}

TArray<int32> AATBTTGridManager::GetIndexesInRangeCpp(int32 StartIndex, int32 Range, bool bDiamondShaped)
{
	TArray<int32> LocalInRangeTiles;

	if (bDiamondShaped)
	{
		for (int32 i = -Range; i <= Range; i++)
		{
			for (int32 j = -Range; j <= Range; j++)
			{
				if (((abs(j) + (abs(i))) <= Range))
				{
					LocalInRangeTiles.Add(i*IndexXCpp + StartIndex + j);
				}
			}
		}
	}
	else
	{
		for (int32 i = -Range; i <= Range; i++)
		{
			for (int32 j = -Range; j <= Range; j++)
			{
				LocalInRangeTiles.Add(i*IndexXCpp + StartIndex + j);
			}
		}
	}

	return LocalInRangeTiles;

}

TSet<int32> AATBTTGridManager::FindVisibleTilesFromTilesInRangeCpp(int32 StartIndex, TArray<int32> InRangeTiles, float MaxZDifference, int32 MinRange, bool bCheckVisibility, float TraceHeight, bool bDiamondShaped)
{
	TSet<int32> LocalInSightTiles;
	for (int32 const& GridIndex : InRangeTiles)
	{
		if (CheckIfTileIsVisibleFromOtherTileCpp(StartIndex, GridIndex, MaxZDifference, MinRange, bCheckVisibility, TraceHeight, bDiamondShaped))
		{
			LocalInSightTiles.Add(GridIndex);
		}
	};

	return LocalInSightTiles;
}

bool AATBTTGridManager::CheckIfTileIsVisibleFromOtherTileCpp(int32 StartIndex, int32 TargetIndex, float MaxZDifference, int32 MinRange, bool bCheckVisibility, float TraceHeight, bool bDiamondShaped)
{
	if (!CheckIfImpassableCpp(TargetIndex))
	{
		if (MinRange == 0)
		{
			if (GridLocationsCpp.Contains(TargetIndex) && (abs(GridLocationsCpp[StartIndex].Z - GridLocationsCpp[TargetIndex].Z) < MaxZDifference))
			{
				if (bCheckVisibility)
				{
					return !TraceOnGridCpp(StartIndex, TargetIndex, TraceHeight);
				}
				else
				{
					return true;
				}
			}
		}
	}

	return false;
}

TSet<int32> AATBTTGridManager::FindTilesInRangeCpp(int32 StartIndex, int32 Range, float MaxZDifference, int32 MinRange, bool bCheckVisibility, float TraceHeight, bool bDiamondShaped)
{
	TArray<int32> LocalInRangeTiles = GetIndexesInRangeCpp(StartIndex, Range, bDiamondShaped);

	return FindVisibleTilesFromTilesInRangeCpp(StartIndex, LocalInRangeTiles, MaxZDifference, MinRange, bCheckVisibility, TraceHeight, bDiamondShaped);
}

void AATBTTGridManager::RunPathfindingCpp(int32 StartIndex, int32 MoveRange, TMap<int32, FPathfindingCpp>& PathsMap)
{
	TMap<int32, FPathfindingCpp> LocalPathsMap;
	TArray<FPathListItem> OpenListTiles;
	TArray<FPathListItem> OpenListChildTiles;
	LocalPathsMap.Add(StartIndex, FPathfindingCpp{ 0, StartIndex });
	OpenListTiles.Add(FPathListItem{ StartIndex, 0, StartIndex });

	for (int32 Step = 0; Step < MoveRange; Step++)
	{
		for (FPathListItem& Item : OpenListTiles)
		{
			if (Step == Item.Cost)
			{
				for (auto const& Edge : GridEdgesCpp.Find(Item.Index)->Edges)
				{
					if (!LocalPathsMap.Contains(Edge.Key))
					{
						if (Edge.Value + Item.Cost <= MoveRange)
						{
							LocalPathsMap.Add(Edge.Key, FPathfindingCpp{ Edge.Value + Item.Cost , Item.Index });
							OpenListChildTiles.Add(FPathListItem{ Edge.Key, Edge.Value + Item.Cost , Item.Index });
						}
					}
				};
			}
		};

		if (OpenListChildTiles.Num() == 0)
		{
			PathsMap = LocalPathsMap;
			return;
		}
		else
		{
			OpenListTiles = OpenListChildTiles;
		}
	};
	PathsMap = LocalPathsMap;
	return;
}

bool AATBTTGridManager::CheckIfImpassableCpp(int32 GridIndex)
{
	if (GridEdgesCpp.Contains(GridIndex))
	{
		if (GridEdgesCpp[GridIndex].Edges.Num() > 0)
		{
			return false;
		}
	}

	return true;
}

int32 AATBTTGridManager::ConvertLocaionToIndexCpp(FVector Location) const
{
	FVector GridLocation = GetTransform().InverseTransformPosition(Location);

	if (HeightmapCpp == Multilevel)
	{
		return (floor(((GridLocation.Y + (TileSizeCpp / 2)) / TileSizeCpp))) + ((floor(((GridLocation.X + (TileSizeCpp / 2)) / TileSizeCpp))) * IndexXCpp) + ((floor(((GridLocation.Z - MinGridHeightCpp) / HeightBetweenLevelsCpp))) * IndexZCpp);
	}
	else
	{
		return (floor(((GridLocation.Y + (TileSizeCpp / 2)) / TileSizeCpp))) + ((floor(((GridLocation.X + (TileSizeCpp / 2)) / TileSizeCpp))) * IndexXCpp);
	}

	return 0;
}

void AATBTTGridManager::UpdateHeightmapCacheCpp(int32 GridIndex)
{
	if (!HeightMapLevelsCpp.Contains(GridIndex % IndexZCpp))
	{
		FNestedInts Empty;
		HeightMapLevelsCpp.Add(GridIndex % IndexZCpp, Empty);
	}

	HeightMapLevelsCpp.Find(GridIndex % IndexZCpp)->Integers.Add(GridIndex / IndexZCpp);

}

TArray<FVector> AATBTTGridManager::ConvertGridIndexesToLocationsCpp(TArray<int32> GridIndexes, FVector Offset)
{
	TArray<FVector> LocalLocations; //TODO: initalize
	
	for (int32 GridIndex : GridIndexes)
	{
		LocalLocations.Add (GridLocationsCpp[GridIndex] +Offset);
	}
	

	return LocalLocations;
}

TArray<int32> AATBTTGridManager::FindPathToIndexCpp(TMap<int32, FPathfindingCpp> PathsMap, int32 EndIndex)
{
	int32 LocalGridIndex = EndIndex;
	TArray<int32> LocalPathIndexes;

	while (PathsMap[LocalGridIndex].Cost != 0)
	{
		LocalPathIndexes.Insert(LocalGridIndex,0);

		LocalGridIndex = PathsMap[LocalGridIndex].Parent;
	}

	LocalPathIndexes.Insert(LocalGridIndex,0);

	return LocalPathIndexes;
}

// Called when the game starts or when spawned
void AATBTTGridManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AATBTTGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

