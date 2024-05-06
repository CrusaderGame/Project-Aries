



#include "ProjectAries/Public/Grid/GridManager.h"




AGridManager::AGridManager()
{
	MySceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	CollisionPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionPlane"));
	MyBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("MyBoxComponent"));
	DefaultTile = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("DefaultTile"));
	DebugMarker = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("DebugMarker"));
	MyDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("MyDecalComponent"));
	
	
	MySceneComponent->SetupAttachment(RootComponent);
	CollisionPlane->SetupAttachment(MySceneComponent);
	MyBoxComponent->SetupAttachment(MySceneComponent);
	DefaultTile->SetupAttachment(MySceneComponent);
	DebugMarker->SetupAttachment(MySceneComponent);
	MyDecalComponent->SetupAttachment(MySceneComponent);
	
	MyBoxComponent->SetBoxExtent(FVector(100, 100, 100));
	
	/// TODO
	/// totalnie do sprawdzenia nie mam pojęcia czy u kogoś innego będzie działać ta ścieżka
	///
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("../Project-Aries/Content/AdvancedTurnBasedTileToolkit/Core/Tiles/SM_Tile_Square.uasset"));
	if (MeshAsset.Succeeded())
	{
		DefaultTile->SetStaticMesh(MeshAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("wrong Path to Asset"));
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Debug(TEXT("../Project-Aries/Content/AdvancedTurnBasedTileToolkit/Core/GridUI/SM_Arrow.uasset"));
	if (MeshAsset.Succeeded())
	{
		DebugMarker->SetStaticMesh(MeshAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("wrong Path to Asset"));
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("../Project-Aries/Content/AdvancedTurnBasedTileToolkit/Core/Grid/M_SquareGrid.uasset"));
	if (DecalMaterialAsset.Succeeded())
	{
		MyDecalComponent->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("wrong Path to Asset"));
	}
	
	PreventIncompatibleVariableCombinations();
}

void AGridManager::PreventIncompatibleVariableCombinations()
{
	 MaxGridHeight;
	 HeightBetweenLevels;
	 int MaxLevels;
	
	MaxLevels = static_cast<int>(MaxGridHeight / HeightBetweenLevels);

	if (HeightSlowIncrement > HeightImpassableCutoff)
	{
		HeightSlowIncrement = HeightImpassableCutoff;
	}
	
	switch(HeightMap)
	{
		case EHeight::False:
		break;

		case EHeight::OneLevel:
		break;

		case EHeight::Multilevel:
		bAutoEdgeCostsBasedOnHeight = true;
	}
	
}
