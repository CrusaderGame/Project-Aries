// Fill out your copyright notice in the Description page of Project Settings.

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
	
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Movement", meta = (AllowPrivateAccess = "true"))
		FVector DesiredLocation;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Movement", meta = (AllowPrivateAccess = "true"))
		float Movement_Interp;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Movement", meta = (AllowPrivateAccess = "true"))
		float Movement_Speed;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Rotation", meta = (AllowPrivateAccess = "true"))
		float DesiredYawRotation;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Rotation", meta = (AllowPrivateAccess = "true"))
		float Rotation_Interp;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Rotation", meta = (AllowPrivateAccess = "true"))
		float Rotation_Speed;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Zoom", meta = (AllowPrivateAccess = "true"))
		float DesiredZoom;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Zoom", meta = (AllowPrivateAccess = "true"))
		float ZoomSpeed;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Zoom", meta = (AllowPrivateAccess = "true"))
		float Zoom_Interp;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Zoom", meta = (AllowPrivateAccess = "true"))
		float MinZoomDistance;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EW Character Zoom", meta = (AllowPrivateAccess = "true"))
		float MaxZoomDistance;

		/*FVector DesiredLocation;
		float Movement_Interp;
		float Movement_Speed;

		float DesiredYawRotation;
		float Rotation_Interp;
		float Rotation_Speed;

		
		float DesiredZoom;
		float ZoomSpeed;
		float Zoom_Interp;
		float MinZoomDistance;
		float MaxZoomDistance;*/
};
