


#include "GridComponent.h"

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
					
					for(int LocalIte; LocalIte < LocalLastIndex; LocalIte++)
					{
						
					}
					
					break;
				}
				

			case EHeight::OneLevel:
				{

					
					break;
				}
				

			case EHeight::Multilevel:
				{

					
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
	
	for(; LocalMaxY <= NumTilesY; ++LocalMaxY)
	{
		for(;LocalMaxX <= NumTilesX; ++LocalMaxX)
		{
			GridIndexes.push_back((LocalMaxX*IndexX)+LocalMaxY+LocalGridIndexOffset);
		}
	}
					
	return GridIndexes ;
}


	