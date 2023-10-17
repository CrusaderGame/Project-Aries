//EW_Character.cpp


#include "EW_Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"


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

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);
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

void AEW_Character::Multicast_SetDesiredLocation_Implementation(const FVector& NewDesiredLocation)
{
	DesiredLocation = NewDesiredLocation;
}

void AEW_Character::Move(const FInputActionValue& Value)
{
	FVector2D MoveInput2D = Value.Get<FVector2D>();
	FVector MoveInput(MoveInput2D.X, MoveInput2D.Y, 0.f); // Convert FVector2D to FVector

	HandleInput(MoveInput, 0.f, 0.f);
	if (!HasAuthority())
	{
		Server_HandleInput(MoveInput, 0.f, 0.f);
	}
}



void AEW_Character::Rotate(const FInputActionValue& Value)
{
	float RotateInput = Value.Get<float>();
	HandleInput(FVector::ZeroVector, RotateInput, 0.f);
	if (!HasAuthority())
	{
		Server_HandleInput(FVector::ZeroVector, RotateInput, 0.f);
	}
}
void AEW_Character::Zoom(const FInputActionValue& Value)
{
	float ZoomInput = Value.Get<float>();
	HandleInput(FVector::ZeroVector, 0.f, ZoomInput);
	if (!HasAuthority())
	{
		Server_HandleInput(FVector::ZeroVector, 0.f, ZoomInput);
	}
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



void AEW_Character::HandleInput(const FVector& MoveInput, const float& RotateInput, const float& ZoomInput)
{
	// Movement logic
	const FRotator YawRotation(0.f, FollowCamera->GetComponentRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	DesiredLocation += (ForwardDirection * MoveInput.X + RightDirection * MoveInput.Y + ForwardDirection * MoveInput.Z) * Movement_Speed;

	if (HasAuthority())
	{
		Multicast_SetDesiredLocation(DesiredLocation);
	}
	else
	{
		// This assumes that your Server_Move function or similar can handle FVector input for movement
		// Server_Move(FInputActionValue(MoveInput));
	}

	// Rotation logic
	DesiredYawRotation += RotateInput * Rotation_Speed;
	SetActorRotation(FRotator(0, DesiredYawRotation, 0));

	// Zoom logic
	DesiredZoom = FMath::Clamp(CameraBoom->TargetArmLength + ZoomInput * ZoomSpeed, MinZoomDistance, MaxZoomDistance);
}

void AEW_Character::Server_HandleInput_Implementation(FVector MoveInput, float RotateInput, float ZoomInput)
{
	HandleInput(MoveInput, RotateInput, ZoomInput);
}
bool AEW_Character::Server_HandleInput_Validate(FVector MoveInput, float RotateInput, float ZoomInput)
{
	return true;
}
