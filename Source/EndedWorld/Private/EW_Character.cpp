//EW_Character.cpp
#include "EW_Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"

AEW_Character::AEW_Character()
	: DesiredLocation(FVector::ZeroVector),
	Movement_Interp(5.0f),
	Movement_Speed(30.0f),
	DesiredYawRotation(0.0f),
	Rotation_Interp(10.0f),
	Rotation_Speed(5.0f),
	Zoom_Interp(5.0f),
	ZoomSpeed(70.0f),
	MinZoomDistance(500.0f),
	MaxZoomDistance(2800.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	InitializeComponents();
}

void AEW_Character::InitializeComponents()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 1800.0f;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(-65.f, 0.f, 0.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->SetIsReplicated(true);

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	DesiredZoom = CameraBoom->TargetArmLength;
}

void AEW_Character::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
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
	FVector MoveInput(MoveInput2D.X, MoveInput2D.Y, 0.f);

	ProcessInput(MoveInput, 0.f, 0.f);
}

void AEW_Character::Rotate(const FInputActionValue& Value)
{
	float RotateInput = Value.Get<float>();
	ProcessInput(FVector::ZeroVector, RotateInput, 0.f);
}

void AEW_Character::Zoom(const FInputActionValue& Value)
{
	float ZoomInput = Value.Get<float>();
	ProcessInput(FVector::ZeroVector, 0.f, ZoomInput);
}

void AEW_Character::ProcessInput(const FVector& MoveInput, const float& RotateInput, const float& ZoomInput)
{
	HandleInput(MoveInput, RotateInput, ZoomInput);
	if (!HasAuthority())
	{
		Server_HandleInput(MoveInput, RotateInput, ZoomInput);
	}
}

void AEW_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMovement(DeltaTime);
	UpdateRotation(DeltaTime);
	UpdateZoom(DeltaTime);
}

void AEW_Character::UpdateMovement(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, DesiredLocation, DeltaTime, Movement_Interp);
	SetActorLocation(NewLocation);
}

void AEW_Character::UpdateRotation(float DeltaTime)
{
	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), FRotator(0, DesiredYawRotation, 0), DeltaTime, Rotation_Interp);
	SetActorRotation(NewRotation);
}

void AEW_Character::UpdateZoom(float DeltaTime)
{
	float CurrentZoom = CameraBoom->TargetArmLength;
	float NewZoom = FMath::FInterpTo(CurrentZoom, DesiredZoom, DeltaTime, Zoom_Interp);
	CameraBoom->TargetArmLength = NewZoom;
}

void AEW_Character::HandleInput(const FVector& MoveInput, const float& RotateInput, const float& ZoomInput)
{
	const FRotator YawRotation(0.f, FollowCamera->GetComponentRotation().Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	DesiredLocation += (ForwardDirection * MoveInput.X + RightDirection * MoveInput.Y) * Movement_Speed;
	if (HasAuthority())
	{
		Multicast_SetDesiredLocation(DesiredLocation);
	}

	DesiredYawRotation += RotateInput * Rotation_Speed;

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
