//EW_Character.h

#pragma once

#include "InputMappingContext.h"
#include "InputAction.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EW_Character.generated.h"


UCLASS()
class ENDEDWORLD_API AEW_Character : public ACharacter
{
	GENERATED_BODY()

public:
	AEW_Character();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetDesiredLocation(const FVector& NewDesiredLocation);
	virtual void Multicast_SetDesiredLocation_Implementation(const FVector& NewDesiredLocation);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_HandleInput(FVector MoveInput, float RotateInput, float ZoomInput);
	virtual bool Server_HandleInput_Validate(FVector MoveInput, float RotateInput, float ZoomInput);
	virtual void Server_HandleInput_Implementation(FVector MoveInput, float RotateInput, float ZoomInput);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* RotateAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ZoomAction;

	void Move(const FInputActionValue& Value);
	void Rotate(const FInputActionValue& Value);
	void Zoom(const FInputActionValue& Value);

private:
		UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* CameraBoom;

		UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* FollowCamera;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* OverheadWidget;



		//MOVE
		UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "EW Character Rotation", meta = (AllowPrivateAccess = "true"))
		FVector DesiredLocation;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Movement", meta = (AllowPrivateAccess = "true"))
		float Movement_Interp;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Movement", meta = (AllowPrivateAccess = "true"))
		float Movement_Speed;

		//ROTATION
		UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "EW Character Rotation", meta = (AllowPrivateAccess = "true"))
		float DesiredYawRotation;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Rotation", meta = (AllowPrivateAccess = "true"))
		float Rotation_Interp;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Rotation", meta = (AllowPrivateAccess = "true"))
		float Rotation_Speed;

		//ZOOM
		UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "EW Character Rotation", meta = (AllowPrivateAccess = "true"))
		float DesiredZoom;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Zoom", meta = (AllowPrivateAccess = "true"))
		float ZoomSpeed;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Zoom", meta = (AllowPrivateAccess = "true"))
		float Zoom_Interp;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Zoom", meta = (AllowPrivateAccess = "true"))
		float MinZoomDistance;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Zoom", meta = (AllowPrivateAccess = "true"))
		float MaxZoomDistance;

		void HandleInput(const FVector& MoveInput, const float& RotateInput, const float& ZoomInput);
};

