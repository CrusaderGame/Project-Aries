// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BCGameState.generated.h"

UENUM(BlueprintType)
enum ETurnState
{
	Setup,
	Running,
	GameOver,
	Other
};
/**
 * 
 */
UCLASS()
class PROJECTARIES_API ABCGameState : public AGameState
{
	GENERATED_BODY()

public:
	void SetTurnState(ETurnState NewTurnState) { TurnState = NewTurnState; }
	ETurnState GetTurnState() const { return TurnState; } 
protected:
private:
	ETurnState TurnState;
};
