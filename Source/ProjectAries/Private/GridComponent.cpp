


#include "GridComponent.h"

#include "ToolContextInterfaces.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UGridComponent::UGridComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGridComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGridComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UGridComponent::CreateGridLocation(int32 StartIndex, int32 NumTilesX, int32 NumTilesY, EHeight HeightMap )
{
	if(NumTilesX * NumTilesY>0)
	{
		switch(HeightMap)
		{
			case EHeight::EFalse:
				{
					std::vector<int> GridIndexes = GetAllGridIndexesNaive(NumTilesX,NumTilesY,StartIndex);

					int32 LocalLastIndex = GridIndexes.size();

					
					for(int LocalIterator =0; LocalIterator < LocalLastIndex; LocalIterator++)
					{
						FGridLocation grid;
						grid.Index = GridIndexes[LocalIterator];
						grid.Vector = ConvertInexToLocationNaive(GridIndexes[LocalIterator]);
						
						GridLocations.Add(grid);
					}
					bSucces = true;
					break;
				}
				

			case EHeight::OneLevel:
				{
					std::vector<int> GridIndexes = GetAllGridIndexesNaive(NumTilesX,NumTilesY,StartIndex);

					int32 LocalLastIndex = GridIndexes.size();

					
					for(int LocalIterator =0; LocalIterator < LocalLastIndex; LocalIterator++)
					{
						
						FGridLocation grid;
						grid.Index = GridIndexes[LocalIterator];
						
						FTransform ActorTransform = GetOwner()->GetActorTransform();
						FVector TransformLocationStart = UKismetMathLibrary::TransformLocation(ActorTransform, FVector(grid.Vector.X,grid.Vector.Y,MaxGridHeight));
						FVector TransformLocationEnd = UKismetMathLibrary::TransformLocation(ActorTransform, FVector(grid.Vector.X,grid.Vector.Y,MinGridHeight));

						FCollisionQueryParams QueryParams;
						QueryParams.AddIgnoredActor(GetOwner());
						FHitResult Hit;
						
						GetWorld()->LineTraceSingleByChannel(
							Hit,
							TransformLocationStart,
							TransformLocationEnd,
							ECC_Visibility,
							QueryParams
							);
						
						grid.Vector = ActorTransform.InverseTransformPosition(Hit.Location);
					
						
						GridLocations.Add(grid);
					}
					bSucces = true;
					
					break;
				}
				

			case EHeight::Multilevel:
				{
					//WORK IN PROGRESS
					
					/*std::vector<int> GridIndexes = GetAllGridIndexesNaive(NumTilesX,NumTilesY,StartIndex);

					int32 LocalLastIndex = GridIndexes.size();

					
					for(int LocalIterator =0; LocalIterator < LocalLastIndex; LocalIterator++)
					{
						FGridLocation grid;
						grid.Index = GridIndexes[LocalIterator];
						grid.Vector = ConvertInexToLocationNaive(GridIndexes[LocalIterator]);
						
						


						
					}*/
					bSucces = true;
					break;
				}
				
		}
	} 
	return bSucces;
}


std::vector<int> UGridComponent::GetAllGridIndexesNaive(int32 NumTilesX, int32 NumTilesY,int32 StartIndex)
{
	
	int LocalGridIndexOffset = StartIndex;
	int LocalMaxY = NumTilesY-1;
	int LocalMaxX = NumTilesX-1;

	std::vector<int> GridIndexes;

	int LocalYiterator =0;
	
	for(; LocalYiterator <= LocalMaxY; LocalYiterator++)
	{
		int LocalXiterator =0;
		
		for(;LocalXiterator <= NumTilesX; ++LocalXiterator)
		{
			GridIndexes.push_back((LocalMaxX*IndexX)+LocalMaxY+LocalGridIndexOffset);
		}
	}
					
	return GridIndexes ;
}


FVector UGridComponent::ConvertInexToLocationNaive(int32 GridIndex)
{

	int X = ((GridIndex/IndexX)%IndexX)*TileXSize;

	int Y = (GridIndex % IndexX)*TileYSize;

	int Z = (GridIndex/IndexZ)*HeightBetweenLevels;
	
	return FVector(X,Y,Z);
}
	
