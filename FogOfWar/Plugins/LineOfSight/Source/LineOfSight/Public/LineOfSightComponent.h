// Copyright (c) 2021 Evgeniy Oshmarin

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "LineOfSightComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("LineOfSight Stat Group"), STATGROUP_LineOfSight, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("LineOfSight Tick (All functions) "), STAT_LineOfSightTick, STATGROUP_LineOfSight, LINEOFSIGHT_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("LineOfSight Line Trace"), STAT_LineOfSightLineTrace, STATGROUP_LineOfSight, LINEOFSIGHT_API);
DECLARE_CYCLE_STAT_EXTERN(TEXT("LineOfSight Build Mesh"), STAT_LineOfSightBuildMesh, STATGROUP_LineOfSight, LINEOFSIGHT_API);


UENUM()
enum class ETypeTriangle : uint8
{
	E_LR            UMETA(DisplayName = "Left -> Right"),
	E_RL            UMETA(DisplayName = "Right -> Left"),
	E_LR_RL         UMETA(DisplayName = "Left -> Right | Right -> Left (Beta)"),
	E_RL_LR         UMETA(DisplayName = "Right -> Left | Left -> Right (Beta)")
};

UENUM()
enum class ETypeArc : uint8
{
	Arc_VectorLenght			UMETA(DisplayName = "Arc"),
	ArcVectorLenghtFlat			UMETA(DisplayName = "Line")
};

UENUM(BlueprintType)
enum class EAxisTypeComp : uint8
{
	Z,
	Y,
	X
};

UENUM()
enum class ETypeRotation : uint8
{
	Relative_Rotation			UMETA(DisplayName = "Relative_Rotation (Gimbal_Lock)"),
	World_Rotation				UMETA(DisplayName = "Alternative method (No Gimbal_Lock for 1 or 2 axes)"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitEveryFrame, const FHitResult&, Hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitStart, const FHitResult&, Hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitEnd, const FHitResult&, Hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRotateToAngleEnd);


UCLASS(ClassGroup = Custom, meta = (BlueprintSpawnableComponent))
class LINEOFSIGHT_API ULineOfSightComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:

	ULineOfSightComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	/* The event is similar to On Component Begin Overlap */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "LineOfSight Component\|Event")
		FHitStart BeginOverlap;

	/* The event is similar to On Component End Overlap */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "LineOfSight Component\|Event")
		FHitEnd EndOverlap;
	 
	/* Disabled by default  */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "LineOfSight Component\|Event")
		FHitEveryFrame HitEveryFrame;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "LineOfSight Component\|Event")
		FRotateToAngleEnd RotateToAngleEnd;


	UPROPERTY(EditAnywhere, Category = "LineOfSight Component")
		ETypeArc GeometryType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight Component")
		float Angle1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight Component")
		float Angle2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight Component")
		float Radius1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight Component")
		float Radius2;

	UPROPERTY(EditAnywhere, Category = "LineOfSight Component")
		ETypeTriangle Type_Of_Triangles;

	UPROPERTY(EditAnywhere, Category = "LineOfSight Component", meta = (ClampMin = "2", EditCondition = "GeometryType == ETypeArc::Arc_VectorLenght"))
		bool ReverseArch1;

	/* Adds an actor to the list for trace exceptions. */
	UPROPERTY(EditAnywhere, Category = "LineOfSight Component")
		bool IgnoreSelf;

	// Calls Event Hit Every Frame when an object is detected
	UPROPERTY(EditAnywhere, Category = "LineOfSight Component", meta = (DisplayName = "HitEveryFrameEvent (Deprecated)"))
		bool HitEveryFrameEvent;

	/* Calls Events: Hit Start and Hit End. Similar to Event On Component Begin Overlap. */
	UPROPERTY(EditAnywhere, Category = "LineOfSight Component")
		bool BeginAndEndOverlapEvent;

	/* Used for optimization. Use false if you only need 1 arch instead of Radius 1 = 0 and Angle 1 = 0. */
	UPROPERTY(EditAnywhere, Category = "LineOfSight Component")
		bool OnlyOneArc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight Component")
		bool Only_Z_Rotation;

	UPROPERTY(EditAnywhere, Category = "LineOfSight Component")
		bool TraceComplex;

#if WITH_EDITORONLY_DATA
	/* Only available in the editor. Draws tracing lines. */
	UPROPERTY(EditAnywhere, Category = "LineOfSight Component")
		bool Debug;

	UPROPERTY(EditAnywhere, Category = "LineOfSight Component")
		float DebugLineThickness = 2.0f;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineOfSight Component")
		UMaterialInterface* Material;

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetAngle1(const float NewAngle);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetAngle2(const float NewAngle);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		FORCEINLINE float GetAngle1() const { return Angle1; };

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		FORCEINLINE float GetAngle2() const { return Angle2; };

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetRadius1(const float NewRadius);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetRadius2(const float NewRadius);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		FORCEINLINE float GetRadius1() const { return Radius1; };

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		FORCEINLINE float GetRadius2() const { return Radius2; };

	bool UseStandardCollision;

	/* This function must be called before Start Build Arc. */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetStandardCollision(const bool NewStatus);

	void InterpAngle();

	bool bInterpAngleBool;

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StartInterpAngle(const float NewAngle1, const float NewAngle2, const float Speed1, const float Speed2);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StopInterpAngle();

	float AngleForInterpAngle1;

	float AngleForInterpAngle2;

	float SpeedInterpAngle1;

	float SpeedInterpAngle2;

	void InterpRadius();

	bool InterpRadiusBool;

	/* Smoothly changes the radius */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StartInterpRadius(const float NewRadius1, const float NewRadius2, const float Speed1, const float Speed2);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StopInterpRadius();

	float Radius1ForInterp;

	float Radius2ForInterp;

	float SpeedInterpRadius;

	float SpeedInterpRadius1;

	float SpeedInterpRadius2;


	UPROPERTY(BlueprintReadWrite, Category = "LineOfSight Component")
		ETypeTriangle TypeTriangle;

	UPROPERTY()
		bool FlipDirection;

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		FORCEINLINE uint8 GetHitEveryFrame() const { return HitEveryFrameEvent; }

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		FORCEINLINE uint8 GetBeginAndEndOverlapEvent() const { return BeginAndEndOverlapEvent; }

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetHitEveryFrame(const bool NewValue);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetBeginAndEndOverlapEvent(const bool NewValue);

	FCollisionQueryParams TraceParams;

	ECollisionChannel TraceChanne2;


	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetTraceChannel(const ETraceTypeQuery NewTraceChanne);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetTraceComplex(const bool NewValue);


	/* It is not recommended to use this function. Change Trace Responces in Collision settings instead */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void AddIgnoredActor(UPARAM(ref) const AActor* ActorToIgnore);

	/* It is not recommended to use this function. Change Trace Responces in Collision settings instead */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void AddActorsToIgnore(UPARAM(ref) TArray<AActor*>& ActorsToIgnore);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void ClearActorsToIgnore(const bool NewIgnoreSelf = true);

	/* Set the Ignore Self option, but clear the actor array added using AddActorsToIgnore */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetIgnoreSelf(const bool NewValueIgnoreSelf);

	void BuildArcVectorLenght();

	void BuildArcVectorLenghtFlat();

	void BuildMesh();

	/* Does TraceLine */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StartLineTrace(ETraceTypeQuery TraceChannel, int32 NumberOfLines = 60);

	bool Cloned;

	UPROPERTY(Transient, DuplicateTransient)
		ULineOfSightComponent* LineOfSightComponentForClone;

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void ZStartCloneTo(ULineOfSightComponent* OtherLineOfSightComponent);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void ZStopCloneTo();

	void CloneTick();

	/* Essentially disables the component. Stops tracing and building the mesh. But the tick function will continue to work. To disable it completely,
	call StopLineTrace () and call SetTickComponentEnabled (false). */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StopLineTrace();

	/* Create Mesh After TraceLine */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StartBuildMesh();

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StopBuildMesh();

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetTickEnable(const bool Enable);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetVisibilityOfMesh(const bool NewVisibility);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		bool MeshIsBuilt() const;

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		bool LineOfSightIsActive() const;

	int16 SlackArray;

	uint8 bRotateInRangeActive;

	uint8 bRotateToAngleActive;

	FRotator AngleRotateRot;

	FRotator AngleRotateRot2;

	FQuat AngleRotateQuat;

	FQuat AngleRotateQuat2;

	float SpeedRotate;

	float Tolerance = 0.0000005f;

	/* Tolerance to compare angles. Default 0.000005f. */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetTolerance(const float NewTolerance);

	EAxisTypeComp AxisForRotateAngle;

	ETypeRotation TypeRotationGlobal;

	void RotateInRange();

	void RotateToAngle();

	/* Maximum angle 89 */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StartRotateInRangeAxis(const EAxisTypeComp Axis, const float Angle, float Speed, const ETypeRotation TypeRotation, const bool NegativeToPositive);

	/* When the rotation is finished, event RotateToAngleEnd will be called. */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StartRotateToAngleAxis(const EAxisTypeComp Axis, const float Angle, const float Speed, const ETypeRotation TypeRotation, const bool AddToCurrent = true);

	/* This function uses World Rotation */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StartRotateToActor(const EAxisTypeComp Axis, AActor* Actor, const float Speed);

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StopRotateToAngle();

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StopRotateInRange();

	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void StopAllRotate();

	float GetAxisValue(EAxisTypeComp Axis, FRotator& Rotator);

	FRotator SetAxisValue(EAxisTypeComp Axis, const float Value, FRotator& Rotator);

	/* Returns the corner to the target. Same as FindLookAtRotation. Works correctly for the Z axis. */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		FRotator FindAngleRotate(UPARAM(ref) AActor*& Actor);

	/* Changes the Geometry Type. Can only be called before Start Line Trace or after Stop Line Trace  */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetGeometryType(const ETypeArc NewType);

	/* Can only be called after Start Build Mesh  */
	UFUNCTION(BlueprintCallable, Category = "LineOfSight Component")
		void SetNormals(const FVector Normals);
	/*
	#if WITH_EDITORONLY_DATA
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PostInitProperties() override;
	#endif
		*/

protected:

	bool bBuildMeshActive;

	bool StartLineTraceActive;

	TArray<FHitResult> WasStartOverlapArray;

	UPROPERTY(Transient)
		TArray<FVector> PointArray1;

	UPROPERTY(Transient)
		TArray<FVector> NormalVertex;

	UPROPERTY(Transient)
		TArray<FVector2D> UV0;

	UPROPERTY(Transient)
		TArray<FLinearColor> VertexColors;

	UPROPERTY(Transient)
		TArray<FProcMeshTangent> Tangents;

	UPROPERTY(Transient)
		TArray<int32> Triangle;

private:

	int Number_Of_Lines;

};
