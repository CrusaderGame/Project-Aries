// Copyright (c) 2021 Evgeniy Oshmarin

#include "LineOfSightComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
//#include "CollisionQueryParams.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_STAT(STAT_LineOfSightTick);
DEFINE_STAT(STAT_LineOfSightLineTrace);
DEFINE_STAT(STAT_LineOfSightBuildMesh);


ULineOfSightComponent::ULineOfSightComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bAutoActivate = true;
	bTickInEditor = true;

	Angle1 = 40;
	Angle2 = 30;
	Radius1 = 70;
	Radius2 = 1000;
	Number_Of_Lines = 60;
	Type_Of_Triangles = ETypeTriangle::E_LR;
	ReverseArch1 = false;
	OnlyOneArc = false;
	Only_Z_Rotation = false;
	TraceComplex = false;
	IgnoreSelf = true;
	HitEveryFrameEvent = false;
	BeginAndEndOverlapEvent = true;

	FlipDirection = false;
	bBuildMeshActive = false;
	bUseComplexAsSimpleCollision = false;
	CastShadow = false;
	SetCollisionProfileName(TEXT("NoCollision"));
	SetGenerateOverlapEvents(false);
	SetCanEverAffectNavigation(false);
	bVisibleInReflectionCaptures = false;
	bVisibleInRayTracing = false;
	bReceiveMobileCSMShadows = false;

	GeometryType = ETypeArc::Arc_VectorLenght;
	UseStandardCollision = false;
	LineOfSightComponentForClone = nullptr;
}

void ULineOfSightComponent::BeginPlay()
{
	Super::BeginPlay();

}

void ULineOfSightComponent::SetBeginAndEndOverlapEvent(const bool NewValue)
{
	BeginAndEndOverlapEvent = NewValue;
	WasStartOverlapArray.Empty();
}

void ULineOfSightComponent::SetHitEveryFrame(const bool NewValue)
{
	HitEveryFrameEvent = NewValue;
}

void ULineOfSightComponent::BuildMesh()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(LineOfSight_Build_Mesh);
	SCOPE_CYCLE_COUNTER(STAT_LineOfSightBuildMesh);

	UpdateMeshSection_LinearColor(0, PointArray1, NormalVertex, UV0, VertexColors, Tangents);
}


void ULineOfSightComponent::StartBuildMesh()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(LineOfSight_StartBuildMesh);

	if (StartLineTraceActive == false)
	{
		UE_LOG(LogTemp, Error, TEXT("LineOfSight Component: The StartBuildMesh function can only be called after the StartLineTrace function."));
		return;
	}

	bBuildMeshActive = true;

	ClearMeshSection(0);
	SetMaterial(0, Material);
	NormalVertex.Reset();
	Triangle.Reset();

	int32 iter = 0;
	if (OnlyOneArc)
	{
		iter = Number_Of_Lines + 2;
	}
	else
	{
		iter = (Number_Of_Lines * 2) + 2;
	}

	NormalVertex.Init(FVector(0.0f, 0.0f, 1.0f), iter);

	if (OnlyOneArc)
	{
		for (int i = 0; i < iter - 1; i++)
		{
			Triangle.Add(0);
			Triangle.Add(i + 2);
			Triangle.Add(i + 1);
		}
	}
	else
	{
		if (Type_Of_Triangles == ETypeTriangle::E_LR)		// Left -> Right
		{
			for (int i = 0; i < iter - 2; i++)
			{
				if (i % 2 == false)
				{
					Triangle.Add(i);
					Triangle.Add(i + 3);
					Triangle.Add(i + 1);
					Triangle.Add(i);
					Triangle.Add(i + 2);
					Triangle.Add(i + 3);
				}
			}
		}
		else if (Type_Of_Triangles == ETypeTriangle::E_RL)	// Right -> Left
		{
			for (int i = 0; i < iter - 2; i++)
			{
				if (i % 2 == false)
				{
					Triangle.Add(i);
					Triangle.Add(i + 2);
					Triangle.Add(i + 1);
					Triangle.Add(i + 1);
					Triangle.Add(i + 2);
					Triangle.Add(i + 3);
				}
			}
		}
		else if (Type_Of_Triangles == ETypeTriangle::E_LR_RL)
		{
			for (int i = 0; i < iter - 2; i++)
			{
				if (i % 2 == false)
				{
					if (i > Number_Of_Lines - 1)
					{
						Triangle.Add(i);
						Triangle.Add(i + 2);
						Triangle.Add(i + 1);
						Triangle.Add(i + 1);
						Triangle.Add(i + 2);
						Triangle.Add(i + 3);
					}
					else
					{
						Triangle.Add(i);
						Triangle.Add(i + 3);
						Triangle.Add(i + 1);
						Triangle.Add(i);
						Triangle.Add(i + 2);
						Triangle.Add(i + 3);
					}
				}
			}
		}
		else if (Type_Of_Triangles == ETypeTriangle::E_RL_LR)
		{
			for (int i = 0; i < iter - 2; i++)
			{
				if (i % 2 == false)
				{
					if (i > Number_Of_Lines - 1)
					{
						Triangle.Add(i);
						Triangle.Add(i + 3);
						Triangle.Add(i + 1);
						Triangle.Add(i);
						Triangle.Add(i + 2);
						Triangle.Add(i + 3);
					}
					else
					{
						Triangle.Add(i);
						Triangle.Add(i + 2);
						Triangle.Add(i + 1);
						Triangle.Add(i + 1);
						Triangle.Add(i + 2);
						Triangle.Add(i + 3);
					}
				}
			}
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Triangle %d"), Triangle.Num());
	CreateMeshSection_LinearColor(0, PointArray1, Triangle, NormalVertex, UV0, VertexColors, Tangents, UseStandardCollision);
}


void ULineOfSightComponent::StopBuildMesh()
{
	bBuildMeshActive = false;
	Cloned = false;
	ClearAllMeshSections();
	NormalVertex.Reset();
	Triangle.Reset();
	//PointArray1.Reset(SlackArray);
}

void ULineOfSightComponent::SetTickEnable(const bool Enable)
{
	StopLineTrace();
	PrimaryComponentTick.SetTickFunctionEnable(Enable);
}

void ULineOfSightComponent::SetVisibilityOfMesh(const bool NewVisibility)
{
	SetMeshSectionVisible(0, NewVisibility);
}

bool ULineOfSightComponent::MeshIsBuilt() const
{
	return bBuildMeshActive;
}

bool ULineOfSightComponent::LineOfSightIsActive() const
{
	return StartLineTraceActive;
}

void ULineOfSightComponent::StartLineTrace(ETraceTypeQuery TraceChannel, int32 NumberOfLines)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(LineOfSight_StartLineTrace);

	StartLineTraceActive = true;
	ClearMeshSection(0);

#if WITH_EDITORONLY_DATA
	if (HitEveryFrameEvent)
	{
		UE_LOG(LogTemp, Warning, TEXT("LineOfSight Component: The HitEveryFrameEvent parameter is deprecated. It will be removed in future releases. Use Begin Overlap and EndOverlap (BeginAndEndOverlapEvent) instead."));
	}
#endif

	if (NumberOfLines < 1)
	{
		NumberOfLines = 1;
	}

	Number_Of_Lines = NumberOfLines;

	if (OnlyOneArc)
	{
		SlackArray = Number_Of_Lines + 2;
	}
	else
	{
		SlackArray = (Number_Of_Lines * 2) + 2;
	}

	PointArray1.Empty();
	PointArray1.Init(FVector(0.0f, 0.0f, 0.0f), SlackArray);

	TraceChanne2 = UEngineTypes::ConvertToCollisionChannel(TraceChannel);
	TraceParams.ClearIgnoredActors();
	TraceParams.ClearIgnoredComponents();
	TraceParams.bTraceComplex = TraceComplex;
	TraceParams.bReturnFaceIndex = false;
	TraceParams.bReturnPhysicalMaterial = false;

	if (IgnoreSelf)
	{
		TraceParams.AddIgnoredActor(GetOwner());
	}
}

void ULineOfSightComponent::ZStartCloneTo(ULineOfSightComponent* OtherLineOfSightComponent)
{
	if (IsValid(OtherLineOfSightComponent))
	{
		LineOfSightComponentForClone = OtherLineOfSightComponent;
		Cloned = true;

		LineOfSightComponentForClone->SetMaterial(0, LineOfSightComponentForClone->Material);
		LineOfSightComponentForClone->CreateMeshSection_LinearColor(0, PointArray1, Triangle, NormalVertex, UV0, VertexColors, Tangents, UseStandardCollision);
	}
}

void ULineOfSightComponent::ZStopCloneTo()
{
	Cloned = false;
	//LineOfSightComponentForClone.Reset();
}

void ULineOfSightComponent::CloneTick()
{
	if (IsValid(LineOfSightComponentForClone))
	{
		LineOfSightComponentForClone->SetWorldRotation(GetComponentRotation());
		LineOfSightComponentForClone->UpdateMeshSection_LinearColor(0, PointArray1, NormalVertex, UV0, VertexColors, Tangents);
	}
	else
	{
		ZStopCloneTo();
	}
}

void ULineOfSightComponent::StopLineTrace()
{
	StartLineTraceActive = false;
	bBuildMeshActive = false;
	PointArray1.Empty();
	WasStartOverlapArray.Empty();
	TraceParams.ClearIgnoredActors();
	TraceParams.ClearIgnoredComponents();
	Cloned = false;
	StopBuildMesh();
}

void ULineOfSightComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(LineOfSight_Tick);
	SCOPE_CYCLE_COUNTER(STAT_LineOfSightTick);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (StartLineTraceActive)
	{
		if (bRotateToAngleActive)
		{
			RotateToAngle();
		}

		if (bRotateInRangeActive)
		{
			RotateInRange();
		}

		if (bInterpAngleBool)
		{
			InterpAngle();
		}

		if (InterpRadiusBool)
		{
			InterpRadius();
		}

		if (GeometryType == ETypeArc::Arc_VectorLenght)
		{
			BuildArcVectorLenght();
		}
		else if (GeometryType == ETypeArc::ArcVectorLenghtFlat)
		{
			BuildArcVectorLenghtFlat();
		}

		if (bBuildMeshActive)
		{
			BuildMesh();
		}

		if (Cloned)
		{
			CloneTick();
		}
	}
}

void ULineOfSightComponent::BeginDestroy()
{
	Super::BeginDestroy();
	TraceParams.ClearIgnoredActors();
	PointArray1.Empty();
	WasStartOverlapArray.Empty();
}

void ULineOfSightComponent::SetAngle1(const float NewAngle)
{
	Angle1 = NewAngle;
}

void ULineOfSightComponent::SetAngle2(const float NewAngle)
{
	Angle2 = NewAngle;
}

void ULineOfSightComponent::SetRadius1(const float NewRadius)
{
	Radius1 = NewRadius;
}

void ULineOfSightComponent::SetRadius2(const float NewRadius)
{
	Radius2 = NewRadius;
}

void ULineOfSightComponent::StartInterpAngle(const float NewAngle1, const float NewAngle2, const float Speed1, const float Speed2)
{
	bInterpAngleBool = true;
	AngleForInterpAngle1 = NewAngle1;
	AngleForInterpAngle2 = NewAngle2;
	SpeedInterpAngle1 = Speed1;
	SpeedInterpAngle2 = Speed2;
}

void ULineOfSightComponent::StopInterpAngle()
{
	bInterpAngleBool = false;
}

void ULineOfSightComponent::InterpRadius()
{
	UWorld* World = GetWorld();
	if (!World) return;
	Radius1 = FMath::FInterpConstantTo(Radius1, Radius1ForInterp, World->GetDeltaSeconds(), SpeedInterpRadius1);
	Radius2 = FMath::FInterpConstantTo(Radius2, Radius2ForInterp, World->GetDeltaSeconds(), SpeedInterpRadius2);

	if (FMath::IsNearlyEqual(Radius1, Radius1ForInterp, Tolerance) && FMath::IsNearlyEqual(Radius2, Radius2ForInterp, Tolerance))
	{
		InterpRadiusBool = false;
	}
}

void ULineOfSightComponent::StartInterpRadius(const float NewRadius1, const float NewRadius2, const float Speed1, const float Speed2)
{
	InterpRadiusBool = true;
	Radius1ForInterp = NewRadius1;
	Radius2ForInterp = NewRadius2;
	SpeedInterpRadius1 = Speed1;
	SpeedInterpRadius2 = Speed2;
}

void ULineOfSightComponent::StopInterpRadius()
{
	InterpRadiusBool = false;
}

void ULineOfSightComponent::BuildArcVectorLenght()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(LineOfSight_Trace);
	SCOPE_CYCLE_COUNTER(STAT_LineOfSightLineTrace);

	PointArray1.Reset(SlackArray);
	if (OnlyOneArc)
	{
		PointArray1.Add(FVector::ZeroVector);
	}

	TSet<TWeakObjectPtr<AActor>> AllHitActorTSet;

	float CurrentAngle1;
	if (!ReverseArch1)
	{
		CurrentAngle1 = Angle1 * -1;
	}
	else
	{
		CurrentAngle1 = (180 - Angle1) + (Angle1 * 2);
	}

	const float IncrementAngle1 = (Angle1 * 2) / Number_Of_Lines;
	float CurrentAngle2 = Angle2 * -1;
	const float IncrementAngle2 = (Angle2 * 2) / Number_Of_Lines;

	const FVector ComponentLocation = GetComponentLocation();

	for (int i = 0; i <= Number_Of_Lines; ++i)
	{
		// arc 1
		FVector ClearPosition1;
		FVector Point1;

		if (OnlyOneArc)
		{
			ClearPosition1 = FVector::ZeroVector;
			Point1 = ComponentLocation;
		}
		else
		{
			float pos1X = FMath::Cos(FMath::DegreesToRadians(CurrentAngle1)) * Radius1;		// x
			float pos1Y = FMath::Sin(FMath::DegreesToRadians(CurrentAngle1)) * Radius1;		// y

			ClearPosition1 = FVector(pos1X, pos1Y, 0);
			pos1X += ComponentLocation.X;
			pos1Y += ComponentLocation.Y;
			Point1 = FVector(pos1X, pos1Y, ComponentLocation.Z);
		}

		// arc 2
		float pos2X = FMath::Cos(FMath::DegreesToRadians(CurrentAngle2)) * Radius2;			// x
		float pos2Y = FMath::Sin(FMath::DegreesToRadians(CurrentAngle2)) * Radius2;			// y

		FVector ClearPosition2(pos2X, pos2Y, 0);
		pos2X += ComponentLocation.X;
		pos2Y += ComponentLocation.Y;
		FVector Point2(pos2X, pos2Y, ComponentLocation.Z);

		const FVector direction1 = (Point1 - ComponentLocation).GetSafeNormal();			// Direction
		const FVector direction2 = (Point2 - ComponentLocation).GetSafeNormal();

		FVector directionRotate1;
		FVector directionRotate2;
		const FRotator RotationComponent = GetComponentRotation();

		// Point 1
		if (OnlyOneArc)
		{
			directionRotate1 = ComponentLocation;
		}
		else
		{
			if (Only_Z_Rotation)
			{
				directionRotate1 = direction1.RotateAngleAxis(RotationComponent.Yaw, FVector(0, 0, 1));							// Z
			}
			else
			{
				//directionRotate1 = UKismetMathLibrary::GreaterGreater_VectorRotator(direction1, RotationComponent);			// SSE Version
				directionRotate1 = direction1.RotateAngleAxis(RotationComponent.Roll * -1, FVector(1, 0, 0));					// X
				directionRotate1 = directionRotate1.RotateAngleAxis(RotationComponent.Pitch * -1, FVector(0, 1, 0));			// Y
				directionRotate1 = directionRotate1.RotateAngleAxis(RotationComponent.Yaw, FVector(0, 0, 1));					// Z
			}
			directionRotate1 = (directionRotate1 * Radius1) + ComponentLocation;
		}

		// Point 2
		if (Only_Z_Rotation)
		{
			directionRotate2 = direction2.RotateAngleAxis(RotationComponent.Yaw, FVector(0, 0, 1));								// Z
		}
		else
		{
			//directionRotate2 = UKismetMathLibrary::GreaterGreater_VectorRotator(direction2, RotationComponent);				// SSE Version
			directionRotate2 = direction2.RotateAngleAxis(RotationComponent.Roll * -1, FVector(1, 0, 0));						// X
			directionRotate2 = directionRotate2.RotateAngleAxis(RotationComponent.Pitch * -1, FVector(0, 1, 0));				// Y
			directionRotate2 = directionRotate2.RotateAngleAxis(RotationComponent.Yaw, FVector(0, 0, 1));						// Z
		}
		directionRotate2 = (directionRotate2 * Radius2) + ComponentLocation;

		TArray<FHitResult> HitResult;
		const bool hit = GetWorld()->LineTraceMultiByChannel(HitResult, directionRotate1, directionRotate2, TraceChanne2, TraceParams);

		if (hit)
		{
			const float distance = UKismetMathLibrary::Vector_Distance(directionRotate1, HitResult.Last().Location);
			//const float distance = FVector::Distance(directionRotate1, HitResult.Last().Location);

			//const FVector DirectionHit = UKismetMathLibrary::GetDirectionUnitVector(ClearPosition1, ClearPosition2);
			const FVector DirectionHit = (ClearPosition2 - ClearPosition1).GetSafeNormal();

			const FVector PositionHit = (DirectionHit * distance) + ClearPosition1;

			if (!OnlyOneArc)
			{
				PointArray1.Add(ClearPosition1);
			}

			PointArray1.Add(PositionHit);
		}
		else
		{
			if (!OnlyOneArc)
			{
				PointArray1.Add(ClearPosition1);
			}
			PointArray1.Add(ClearPosition2);
		}

		if (BeginAndEndOverlapEvent || HitEveryFrameEvent)		// Begin Overlap
		{
			for (auto& val : HitResult)
			{
				if (AllHitActorTSet.Contains(val.GetActor()) == false)
				{
					AllHitActorTSet.Add(val.GetActor());

					if (HitEveryFrameEvent)
					{
						HitEveryFrame.Broadcast(val);
					}

					if (!WasStartOverlapArray.ContainsByPredicate([&](FHitResult& HitResult) {
						return HitResult.GetActor() == val.GetActor();
					}))
					{
						WasStartOverlapArray.Add(val);
						//HitResultHitStart.Add(val);
						BeginOverlap.Broadcast(val);				// Run Delegate
					}
				}
			}
		}

#if WITH_EDITORONLY_DATA
		if (Debug)
		{
			FVector HitLocation = (hit) ? HitResult.Last().Location : directionRotate2;
			DrawDebugLine(GetWorld(), directionRotate1, HitLocation, FColor::Orange, false, -1.f, 0, 1);
		}
#endif

		CurrentAngle1 = (!ReverseArch1) ? CurrentAngle1 += IncrementAngle1 : CurrentAngle1 -= IncrementAngle1;	// if ReverseArch1 then subtract IncrementAngle1
		CurrentAngle2 += IncrementAngle2;
	}

	if (BeginAndEndOverlapEvent)
	{
		for (int32 i = WasStartOverlapArray.Num() - 1; i >= 0; --i)
		{
			if (AllHitActorTSet.Contains(WasStartOverlapArray[i].GetActor()) == false)
			{
				if (WasStartOverlapArray[i].GetActor())
				{
					EndOverlap.Broadcast(WasStartOverlapArray[i]);		// Run Delegate
				}
				WasStartOverlapArray.RemoveAt(i, 1, false);
			}
		}
	}
	/*WasStartOverlapArray.RemoveAllSwap([&](FHitResult& HitResult) {
		return (AllHitActorTSet.Contains(HitResult.Actor) == false && !HitResult.Actor.IsValid() == false);
		}, false);
	*/
	WasStartOverlapArray.Shrink();
}

void ULineOfSightComponent::BuildArcVectorLenghtFlat()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(LineOfSight_Trace);
	SCOPE_CYCLE_COUNTER(STAT_LineOfSightLineTrace);

	PointArray1.Reset(SlackArray);

	if (OnlyOneArc)
	{
		PointArray1.Add(FVector::ZeroVector);
	}

	TSet<TWeakObjectPtr<AActor>> AllHitActorTSet;
	const FVector ComponentLocation = GetComponentLocation();

	float Step1;
	FVector direction1Neg;
	FVector direction1Pos;
	float posNegative1XOrig = 0;
	float posNegative1YOrig = 0;

	if (OnlyOneArc)
	{
		Step1 = 0.0f;
	}
	else
	{
		float posNegative1X = FMath::Cos(FMath::DegreesToRadians(Angle1 * -1)) * Radius1;			// x
		float posNegative1Y = FMath::Sin(FMath::DegreesToRadians(Angle1 * -1)) * Radius1;			// y

		float posPositive1X = FMath::Cos(FMath::DegreesToRadians(Angle1)) * Radius1;				// x
		float posPositive1Y = FMath::Sin(FMath::DegreesToRadians(Angle1)) * Radius1;				// y

		posNegative1XOrig = posNegative1X;		// For Clear Position
		posNegative1YOrig = posNegative1Y;

		posNegative1X += ComponentLocation.X;
		posNegative1Y += ComponentLocation.Y;
		posPositive1X += ComponentLocation.X;
		posPositive1Y += ComponentLocation.Y;

		FVector Point1Neg(posNegative1X, posNegative1Y, ComponentLocation.Z);
		FVector Point1Pos(posPositive1X, posPositive1Y, ComponentLocation.Z);

		float Distance1 = FMath::Abs(posPositive1Y - posNegative1Y);
		Step1 = Distance1 / Number_Of_Lines;

		direction1Neg = (Point1Neg - ComponentLocation).GetSafeNormal();
		direction1Pos = (Point1Pos - ComponentLocation).GetSafeNormal();
	}

	float posNegative2X = FMath::Cos(FMath::DegreesToRadians(Angle2 * -1)) * Radius2;			// x
	float posNegative2Y = FMath::Sin(FMath::DegreesToRadians(Angle2 * -1)) * Radius2;			// y

	float posPositive2X = FMath::Cos(FMath::DegreesToRadians(Angle2)) * Radius2;				// x
	float posPositive2Y = FMath::Sin(FMath::DegreesToRadians(Angle2)) * Radius2;				// y

	const float posNegative2XOrig = posNegative2X;		// For Clear Position
	const float posNegative2YOrig = posNegative2Y;

	posNegative2X += ComponentLocation.X;
	posNegative2Y += ComponentLocation.Y;
	posPositive2X += ComponentLocation.X;
	posPositive2Y += ComponentLocation.Y;

	const FVector Point2Neg(posNegative2X, posNegative2Y, ComponentLocation.Z);
	const FVector Point2Pos(posPositive2X, posPositive2Y, ComponentLocation.Z);

	const float Distance2 = FMath::Abs(posPositive2Y - posNegative2Y);
	const float Step2 = Distance2 / Number_Of_Lines;

	const FVector direction2Neg = (Point2Neg - ComponentLocation).GetSafeNormal();
	const FVector direction2Pos = (Point2Pos - ComponentLocation).GetSafeNormal();

	FVector directionRotate1Neg;
	FVector directionRotate1Pos;
	const FRotator RotationComponent = GetComponentRotation();

	// Point 1
	if (OnlyOneArc)
	{
		directionRotate1Neg = ComponentLocation;
		directionRotate1Pos = ComponentLocation;
	}
	else
	{
		directionRotate1Neg = direction1Neg.RotateAngleAxis(RotationComponent.Roll * -1, FVector(1, 0, 0));				// X
		directionRotate1Neg = directionRotate1Neg.RotateAngleAxis(RotationComponent.Pitch * -1, FVector(0, 1, 0));		// Y
		directionRotate1Neg = directionRotate1Neg.RotateAngleAxis(RotationComponent.Yaw, FVector(0, 0, 1));				// Z
		directionRotate1Neg = (directionRotate1Neg * Radius1) + ComponentLocation;

		directionRotate1Pos = direction1Pos.RotateAngleAxis(RotationComponent.Roll * -1, FVector(1, 0, 0));				// X
		directionRotate1Pos = directionRotate1Pos.RotateAngleAxis(RotationComponent.Pitch * -1, FVector(0, 1, 0));		// Y
		directionRotate1Pos = directionRotate1Pos.RotateAngleAxis(RotationComponent.Yaw, FVector(0, 0, 1));				// Z
		directionRotate1Pos = (directionRotate1Pos * Radius1) + ComponentLocation;
	}

	// Points 2
	FVector directionRotate2Neg = direction2Neg.RotateAngleAxis(RotationComponent.Roll * -1, FVector(1, 0, 0));			// X
	directionRotate2Neg = directionRotate2Neg.RotateAngleAxis(RotationComponent.Pitch * -1, FVector(0, 1, 0));			// Y
	directionRotate2Neg = directionRotate2Neg.RotateAngleAxis(RotationComponent.Yaw, FVector(0, 0, 1));					// Z
	directionRotate2Neg = (directionRotate2Neg * Radius2) + ComponentLocation;

	FVector directionRotate2Pos = direction2Pos.RotateAngleAxis(RotationComponent.Roll * -1, FVector(1, 0, 0));			// X
	directionRotate2Pos = directionRotate2Pos.RotateAngleAxis(RotationComponent.Pitch * -1, FVector(0, 1, 0));			// Y
	directionRotate2Pos = directionRotate2Pos.RotateAngleAxis(RotationComponent.Yaw, FVector(0, 0, 1));					// Z
	directionRotate2Pos = (directionRotate2Pos * Radius2) + ComponentLocation;

	const FVector Direction1AfterRotation = (directionRotate1Pos - directionRotate1Neg).GetSafeNormal();		// UKismetMathLibrary::GetDirectionUnitVector(GetComponentLocation(), Point1);
	const FVector Direction2AfterRotation = (directionRotate2Pos - directionRotate2Neg).GetSafeNormal();

	for (int i = 0; i <= Number_Of_Lines; i++)
	{
		FVector direction1Step;
		if (OnlyOneArc)
		{
			direction1Step = GetComponentLocation();
		}
		else
		{
			direction1Step = (Direction1AfterRotation * (Step1 * i)) + directionRotate1Neg;
		}

		FVector direction2Step = (Direction2AfterRotation * (Step2 * i)) + directionRotate2Neg;

		FVector ClearPosition1(posNegative1XOrig, posNegative1YOrig + (Step1 * i), 0);
		FVector ClearPosition2(posNegative2XOrig, posNegative2YOrig + (Step2 * i), 0);
		if (OnlyOneArc)
		{
			ClearPosition1 = FVector::ZeroVector;
		}

		TArray< FHitResult> HitResult;
		const bool hit = GetWorld()->LineTraceMultiByChannel(HitResult, direction1Step, direction2Step, TraceChanne2, TraceParams);

		if (BeginAndEndOverlapEvent || HitEveryFrameEvent)		// Begin Overlap
		{
			for (auto& val : HitResult)
			{
				if (AllHitActorTSet.Contains(val.GetActor()) == false)
				{
					AllHitActorTSet.Add(val.GetActor());

					if (HitEveryFrameEvent)
					{
						HitEveryFrame.Broadcast(val);
					}

					if (!WasStartOverlapArray.ContainsByPredicate([&](FHitResult& HitResult) {
						return HitResult.GetActor() == val.GetActor();
					}))
					{
						WasStartOverlapArray.Add(val);
						BeginOverlap.Broadcast(val);				// Run Delegate
					}
				}
			}
		}

		if (hit)
		{
			const float distance = UKismetMathLibrary::Vector_Distance(direction1Step, HitResult.Last().Location);
			const FVector DirectionHit = UKismetMathLibrary::GetDirectionUnitVector(ClearPosition1, ClearPosition2);
			const FVector PositionHit = (DirectionHit * distance) + ClearPosition1;

			if (!OnlyOneArc)
			{
				//PointArray1.Add(direction1Step - ComponentLocation);
				PointArray1.Add(ClearPosition1);
			}

			//PointArray1.Add(ClearPosition1);
			PointArray1.Add(PositionHit);
		}
		else
		{
			if (!OnlyOneArc)
			{
				PointArray1.Add(ClearPosition1);
			}

			PointArray1.Add(ClearPosition2);
		}

#if WITH_EDITORONLY_DATA
		if (Debug)
		{
			FVector HitLocation = (hit) ? HitResult.Last().Location : direction2Step;
			DrawDebugLine(GetWorld(), direction1Step, HitLocation, FColor::Orange, false, -1.f, 0, 1);
		}
#endif
	}

	if (BeginAndEndOverlapEvent)
	{
		for (int32 i = WasStartOverlapArray.Num() - 1; i >= 0; --i)
		{
			if (AllHitActorTSet.Contains(WasStartOverlapArray[i].GetActor()) == false)
			{
				if (WasStartOverlapArray[i].GetActor())
				{
					EndOverlap.Broadcast(WasStartOverlapArray[i]);		// Run Delegate
				}
				WasStartOverlapArray.RemoveAt(i, 1, false);
			}
		}
	}
}

void ULineOfSightComponent::SetStandardCollision(const bool NewStatus)
{
	UseStandardCollision = NewStatus;
}

void ULineOfSightComponent::InterpAngle()
{
	Angle1 = FMath::FInterpConstantTo(Angle1, AngleForInterpAngle1, GetWorld()->GetDeltaSeconds(), SpeedInterpAngle1);
	Angle2 = FMath::FInterpConstantTo(Angle2, AngleForInterpAngle2, GetWorld()->GetDeltaSeconds(), SpeedInterpAngle2);

	if (FMath::IsNearlyEqual(Angle1, AngleForInterpAngle1, Tolerance) && FMath::IsNearlyEqual(Angle2, AngleForInterpAngle2, Tolerance))
	{
		bInterpAngleBool = false;
	}
}

void ULineOfSightComponent::SetTolerance(const float NewTolerance)
{
	Tolerance = NewTolerance;
}


void ULineOfSightComponent::RotateInRange()
{
	FQuat CurrentQuat;

	if (TypeRotationGlobal == ETypeRotation::Relative_Rotation)
	{
		CurrentQuat = GetRelativeRotation().Quaternion();
	}
	else
	{
		CurrentQuat = GetComponentRotation().Quaternion();
	}

	FQuat NewQuat;
	if (FlipDirection)
	{
		NewQuat = FMath::QInterpConstantTo(CurrentQuat, AngleRotateQuat, GetWorld()->GetDeltaSeconds(), SpeedRotate);
		if (AngleRotateQuat.Equals(NewQuat, Tolerance))
		{
			FlipDirection = false;
		}
	}
	else
	{
		NewQuat = FMath::QInterpConstantTo(CurrentQuat, AngleRotateQuat2, GetWorld()->GetDeltaSeconds(), SpeedRotate);
		if (AngleRotateQuat2.Equals(NewQuat, Tolerance))
		{
			FlipDirection = true;
		}
	}

	if (TypeRotationGlobal == ETypeRotation::Relative_Rotation)
	{
		SetRelativeRotation(NewQuat);
	}
	else
	{
		SetWorldRotation(NewQuat);
	}
}


void ULineOfSightComponent::RotateToAngle()
{
	FQuat CurrentQuat;
	if (TypeRotationGlobal == ETypeRotation::Relative_Rotation)
	{
		CurrentQuat = GetRelativeRotation().Quaternion();
	}
	else
	{
		CurrentQuat = GetComponentRotation().Quaternion();
	}

	FQuat NewQuat = FMath::QInterpConstantTo(CurrentQuat, AngleRotateQuat, GetWorld()->GetDeltaSeconds(), SpeedRotate);
	if (TypeRotationGlobal == ETypeRotation::Relative_Rotation)
	{
		SetRelativeRotation(NewQuat);
	}
	else
	{
		SetWorldRotation(NewQuat);
	}

	if (AngleRotateQuat.Equals(NewQuat, Tolerance))
	{
		bRotateToAngleActive = false;
		RotateToAngleEnd.Broadcast();
	}
}

void ULineOfSightComponent::StartRotateInRangeAxis(const EAxisTypeComp Axis, const float Angle, float Speed, const ETypeRotation TypeRotation, const bool NegativeToPositive)
{
	FlipDirection = NegativeToPositive;
	AxisForRotateAngle = Axis;
	bRotateToAngleActive = false;
	bRotateInRangeActive = true;
	SpeedRotate = Speed;
	TypeRotationGlobal = TypeRotation;

	FRotator Clear(0.0f, 0.0f, 0.0f);
	FRotator Delta = SetAxisValue(Axis, Angle, Clear);
	FRotator Delta2 = SetAxisValue(Axis, Angle * -1, Clear);

	FRotator CurrentRotator;
	if (TypeRotation == ETypeRotation::World_Rotation)		// World Rotation
	{
		FQuat Current(GetComponentRotation());
		CurrentRotator = GetComponentRotation();

		FQuat DeltaQuat = Delta.Quaternion();
		FQuat DeltaQuat2 = Delta2.Quaternion();
		AngleRotateQuat = Current * DeltaQuat;				// World
		AngleRotateQuat2 = Current * DeltaQuat2;
	}
	else if (TypeRotation == ETypeRotation::Relative_Rotation)
	{
		FRotator Current(GetRelativeRotation());
		FRotator DeltaRotator = Current + Delta;
		FRotator DeltaRotator2 = Current - Delta;
		AngleRotateQuat = DeltaRotator.Quaternion();		// Relaltive
		AngleRotateQuat2 = DeltaRotator2.Quaternion();
	}
}

void ULineOfSightComponent::StartRotateToAngleAxis(const EAxisTypeComp Axis, const float Angle, const float Speed, const ETypeRotation TypeRotation, const bool AddToCurrent)
{
	AxisForRotateAngle = Axis;
	bRotateInRangeActive = false;
	bRotateToAngleActive = true;
	SpeedRotate = Speed;
	TypeRotationGlobal = TypeRotation;

	if (AddToCurrent)
	{
		FRotator Clear(0.0f, 0.0f, 0.0f);
		FRotator Delta = SetAxisValue(Axis, Angle, Clear);

		// World Rotation
		if (TypeRotation == ETypeRotation::World_Rotation)
		{
			FQuat Current(GetComponentRotation());
			FQuat DeltaQuat = Delta.Quaternion();
			AngleRotateQuat = Current * DeltaQuat;
		}
		else if (TypeRotation == ETypeRotation::Relative_Rotation)
		{
			FRotator Current(GetRelativeRotation());
			FRotator DeltaRotator = Current + Delta;
			AngleRotateQuat = DeltaRotator.Quaternion();
		}
	}
	else
	{
		if (TypeRotation == ETypeRotation::World_Rotation)
		{
			FRotator Current(GetComponentRotation());
			FRotator DeltaRotator = SetAxisValue(Axis, Angle, Current);
			AngleRotateQuat = DeltaRotator.Quaternion();
		}
		else if (TypeRotation == ETypeRotation::Relative_Rotation)
		{
			FRotator Current(GetRelativeRotation());
			FRotator DeltaRotator = SetAxisValue(Axis, Angle, Current);
			AngleRotateQuat = DeltaRotator.Quaternion();
		}
	}
}

void ULineOfSightComponent::StartRotateToActor(const EAxisTypeComp Axis, AActor* Actor, const float Speed)
{
	if (IsValid(Actor))
	{
		AxisForRotateAngle = Axis;
		bRotateInRangeActive = false;
		bRotateToAngleActive = true;

		FRotator AngleTo = FindAngleRotate(Actor);
		float Angle = GetAxisValue(Axis, AngleTo);

		SpeedRotate = Speed;
		TypeRotationGlobal = ETypeRotation::World_Rotation;

		FRotator Clear(0.0f, 0.0f, 0.0f);
		FRotator Delta = SetAxisValue(Axis, Angle, Clear);

		FRotator Current(GetComponentRotation());
		FRotator DeltaRotator = SetAxisValue(Axis, Angle, Current);
		AngleRotateQuat = DeltaRotator.Quaternion();
	}
}


void ULineOfSightComponent::SetGeometryType(const ETypeArc NewType)
{
	if (!StartLineTraceActive)
	{
		GeometryType = NewType;
	}
}

void ULineOfSightComponent::SetNormals(const FVector Normals)
{
	if (StartLineTraceActive)
	{
		NormalVertex.Reset(SlackArray);
		NormalVertex.Init(Normals, SlackArray);
	}
}

void ULineOfSightComponent::StopRotateToAngle()
{
	bRotateToAngleActive = false;
}

void ULineOfSightComponent::StopRotateInRange()
{
	bRotateInRangeActive = false;
}

FRotator ULineOfSightComponent::FindAngleRotate(UPARAM(ref)AActor*& Actor)
{
	if (!IsValid(Actor))
	{
		return FRotator::ZeroRotator;
	}
	FVector vec1 = Actor->GetActorLocation() - GetComponentLocation();
	FRotator rot1 = FRotationMatrix::MakeFromX(vec1).Rotator();
	return rot1;
}

void ULineOfSightComponent::SetTraceChannel(const ETraceTypeQuery NewTraceChanne)
{
	TraceChanne2 = UEngineTypes::ConvertToCollisionChannel(NewTraceChanne);
}

void ULineOfSightComponent::AddActorsToIgnore(UPARAM(ref)TArray<AActor*>& ActorsToIgnore)
{
	TraceParams.AddIgnoredActors(ActorsToIgnore);
}

void ULineOfSightComponent::AddIgnoredActor(UPARAM(ref) const AActor* ActorToIgnore)
{
	TraceParams.AddIgnoredActor(ActorToIgnore);
}

void ULineOfSightComponent::ClearActorsToIgnore(const bool NewIgnoreSelf)
{
	TraceParams.ClearIgnoredActors();
	if (NewIgnoreSelf)
	{
		TraceParams.AddIgnoredActor(GetOwner());
	}
}

void ULineOfSightComponent::SetIgnoreSelf(const bool NewValueIgnoreSelf)
{
	TraceParams.ClearIgnoredActors();
	TraceParams.AddIgnoredActor(GetOwner());
	IgnoreSelf = NewValueIgnoreSelf;
}


void ULineOfSightComponent::StopAllRotate()
{
	bRotateInRangeActive = false;
	bRotateToAngleActive = false;
}

float ULineOfSightComponent::GetAxisValue(EAxisTypeComp Axis, FRotator& Rotator)
{
	if (Axis == EAxisTypeComp::Z)
	{
		return Rotator.Yaw;
	}
	else if (Axis == EAxisTypeComp::Y)
	{
		return Rotator.Pitch;
	}
	else if (Axis == EAxisTypeComp::X)
	{
		return Rotator.Roll;
	}

	return 0.0f;
}

FRotator ULineOfSightComponent::SetAxisValue(EAxisTypeComp Axis, const float Value, FRotator& Rotator)
{
	if (Axis == EAxisTypeComp::Z)
	{
		Rotator.SetComponentForAxis(EAxis::Z, Value);
	}
	else if (Axis == EAxisTypeComp::Y)
	{
		Rotator.SetComponentForAxis(EAxis::Y, Value);
	}
	else if (Axis == EAxisTypeComp::X)
	{
		Rotator.SetComponentForAxis(EAxis::X, Value);
	}

	return Rotator;
}

void ULineOfSightComponent::SetTraceComplex(const bool NewValue)
{
	TraceParams.bTraceComplex = NewValue;
}

/*
#if WITH_EDITORONLY_DATA
void ULineOfSightComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (LineOfSightIsActive())
	{
		//StopLineTrace();
		//StopBuildMesh();
		//StartLineTrace(UEngineTypes::ConvertToTraceType(TraceChanne2));
	}
}
void ULineOfSightComponent::PostInitProperties()
{
	Super::PostInitProperties();
	//StopBuildMesh();
	//StopLineTrace();
}

#endif
*/