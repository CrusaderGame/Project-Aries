

#pragma once

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

/**
 * 
 */
UENUM(BlueprintType) 
enum class EHeight : uint8 {
	OneLevel UMETA(DisplayName = "One Level"),
	Multilevel UMETA(DisplayName = "Multi Level"),
	EFalse UMETA(DisplayName = "EFalse"),
};

UCLASS()
class PROJECTARIES_API AGridManager : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere,Category="")
	USceneComponent* MySceneComponent;
	UPROPERTY(EditAnywhere,Category="")
	UStaticMeshComponent* CollisionPlane;
	UPROPERTY(EditAnywhere,Category="")
	UBoxComponent* MyBoxComponent;
	UPROPERTY(EditAnywhere,Category="")
	UHierarchicalInstancedStaticMeshComponent* DefaultTile;
	UPROPERTY(EditAnywhere,Category="")
	UHierarchicalInstancedStaticMeshComponent* DebugMarker;
	UPROPERTY(EditAnywhere,Category="")
	UDecalComponent* MyDecalComponent;
	
public:
	 AGridManager();
	UFUNCTION(BlueprintCallable)
	void PreventIncompatibleVariableCombinations();

public:
	UPROPERTY(BlueprintReadWrite)
	float MaxGridHeight = 1000.f;
	UPROPERTY(BlueprintReadWrite)
	float HeightBetweenLevels = 200.f;
	UPROPERTY(BlueprintReadWrite)
	double HeightImpassableCutoff = 100; ;
	UPROPERTY(BlueprintReadWrite)
	double HeightSlowIncrement = 100;
	UPROPERTY(BlueprintReadWrite)
	bool bAutoEdgeCostsBasedOnHeight = false;
	UPROPERTY(BlueprintReadWrite)
	EHeight HeightMap = EHeight::EFalse;
private:
	


};
