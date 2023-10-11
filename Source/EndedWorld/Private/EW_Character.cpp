// Fill out your copyright notice in the Description page of Project Settings.


#include "EW_Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <GameFramework/CharacterMovementComponent.h>

AEW_Character::AEW_Character()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 1800.0f;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = false;
	//CameraBoom->SocketOffset = FVector(0.f, 0.f, 300.f);
	CameraBoom->SetRelativeRotation(FRotator(-65.f, 0.f, 0.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	//bOrientRotationToMovement = false;

	
	DesiredLocation = FVector::ZeroVector;
	Movement_Interp = 5.0f;
	Movement_Speed = 30.0f;

	
	DesiredYawRotation = 0.0f;
	Rotation_Interp = 10.0f;
	Rotation_Speed = 5.0f;

	DesiredZoom = CameraBoom->TargetArmLength;
	Zoom_Interp = 5.0f;
	ZoomSpeed = 70.0f;
	MinZoomDistance = 500.0f;
	MaxZoomDistance = 2800.0f;
}

void AEW_Character::BeginPlay()
{
	Super::BeginPlay();


	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{

			Subsystem->AddMappingContext(InputMappingContext,0);

		}
	}


}

void AEW_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEW_Character::Move);
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &AEW_Character::Rotate);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AEW_Character::Zoom);
	}

}

void AEW_Character::Move(const FInputActionValue& Value)
{
	const FVector2D CurrentValue = Value.Get<FVector2D>();

	const FRotator YawRotation(0.f, FollowCamera->GetComponentRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	DesiredLocation += (ForwardDirection * CurrentValue.X + RightDirection * CurrentValue.Y) * Movement_Speed;
}


void AEW_Character::Rotate(const FInputActionValue& Value)
{
	float CurrentValue = Value.Get<float>();
	DesiredYawRotation += CurrentValue * Rotation_Speed;
	SetActorRotation(FRotator(0, DesiredYawRotation, 0));
}

void AEW_Character::Zoom(const FInputActionValue& Value)
{
	float CurrentValue = Value.Get<float>();
	DesiredZoom = FMath::Clamp(CameraBoom->TargetArmLength + CurrentValue * ZoomSpeed, MinZoomDistance, MaxZoomDistance);

}


void AEW_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();
	float InterpolationSpeed = 5.0f; 
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, DesiredLocation, DeltaTime, InterpolationSpeed);
	SetActorLocation(NewLocation);


	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), FRotator(0, DesiredYawRotation, 0), DeltaTime, Rotation_Interp);
	SetActorRotation(NewRotation);



	float CurrentZoom = CameraBoom->TargetArmLength;
	float NewZoom = FMath::FInterpTo(CurrentZoom, DesiredZoom, DeltaTime, Zoom_Interp);
	CameraBoom->TargetArmLength = NewZoom;
}