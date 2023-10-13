// Fill out your copyright notice in the Description page of Project Settings.


#include "EW_Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Net/UnrealNetwork.h"


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
	GetCharacterMovement()->SetIsReplicated(true);

	
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

	bReplicates = true;
	//bReplicateMovement = true;
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

void AEW_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEW_Character, DesiredLocation);
	DOREPLIFETIME(AEW_Character, DesiredYawRotation);
	DOREPLIFETIME(AEW_Character, DesiredZoom);
}

void AEW_Character::PerformMove(const FInputActionValue& Value)
{
	const FVector2D CurrentValue = Value.Get<FVector2D>();
	const FRotator YawRotation(0.f, FollowCamera->GetComponentRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	DesiredLocation += (ForwardDirection * CurrentValue.X + RightDirection * CurrentValue.Y) * Movement_Speed;

	Multicast_UpdateMove(DesiredLocation);
}

void AEW_Character::Move(const FInputActionValue& Value)
{
	if (HasAuthority())
	{
		PerformMove(Value);
	}
	else
	{
		Server_Move(Value);
	}
}

void AEW_Character::Rotate(const FInputActionValue& Value)
{
	if (!HasAuthority())
	{
		Server_Rotate(Value);
	}

	float CurrentValue = Value.Get<float>();
	DesiredYawRotation += CurrentValue * Rotation_Speed;
	SetActorRotation(FRotator(0, DesiredYawRotation, 0));
}
void AEW_Character::Zoom(const FInputActionValue& Value)
{
	if (!HasAuthority())
	{
		Server_Zoom(Value);
	}

	float CurrentValue = Value.Get<float>();
	DesiredZoom = FMath::Clamp(CameraBoom->TargetArmLength + CurrentValue * ZoomSpeed, MinZoomDistance, MaxZoomDistance);

}


void AEW_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move for server and client
	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, DesiredLocation, DeltaTime, Movement_Interp);
	SetActorLocation(NewLocation);

	// Rotation
	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), FRotator(0, DesiredYawRotation, 0), DeltaTime, Rotation_Interp);
	SetActorRotation(NewRotation);

	// Zoom
	float CurrentZoom = CameraBoom->TargetArmLength;
	float NewZoom = FMath::FInterpTo(CurrentZoom, DesiredZoom, DeltaTime, Zoom_Interp);
	CameraBoom->TargetArmLength = NewZoom;
}


//TODO
bool AEW_Character::Server_Rotate_Validate(const FInputActionValue& Value)
{
	return true; 
}
void AEW_Character::Server_Rotate_Implementation(const FInputActionValue& Value)
{
	Rotate(Value);
}


//TODO
bool AEW_Character::Server_Zoom_Validate(const FInputActionValue& Value)
{
	return true;
}
void AEW_Character::Server_Zoom_Implementation(const FInputActionValue& Value)
{
	Zoom(Value);
}

//TODO
bool AEW_Character::Server_Move_Validate(const FInputActionValue& Value)
{
	return true;
}
void AEW_Character::Server_Move_Implementation(const FInputActionValue& Value)
{
	PerformMove(Value);
}
void AEW_Character::Multicast_UpdateMove_Implementation(const FVector& NewDesiredLocation)
{
	DesiredLocation = NewDesiredLocation;

	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, DesiredLocation, GetWorld()->GetDeltaSeconds(), Movement_Interp);
	SetActorLocation(NewLocation);
}
