#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AerodynamicSurface.h"

#include "AeroPhysicsComponent.generated.h"

USTRUCT(BlueprintType)
struct FWheelSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Wheel Settings")
	float WheelRadius;

	UPROPERTY(EditAnywhere, Category = "Wheel Settings")
	float WheelWidth;

	UPROPERTY(EditAnywhere, Category = "Wheel Settings")
	bool bAffectedBySteering;

	UPROPERTY(EditAnywhere, Category = "Wheel Settings")
	float MaxSteerAngle;

	UPROPERTY(EditAnywhere, Category = "Wheel Settings")
	bool bAffectedByBrake;

	UPROPERTY(EditAnywhere, Category = "Wheel Settings")
	float WheelFrictionRatio = 0.7;

	UPROPERTY(EditAnywhere, Category = "Wheel Settings")
	float WheelDragRatio = 0.05;
};

USTRUCT(BlueprintType)
struct FSuspensionSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Suspension Settings")
	FVector SuspensionAxis = FVector(0.0f, 0.0f, -1.0f);

	UPROPERTY(EditAnywhere, Category = "Suspension Settings")
	float SuspensionMaxRaise = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Suspension Settings")
	float SuspensionMaxDrop = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Suspension Settings")
	float SuspensionDampingRatio = 0.5;
	
	UPROPERTY(EditAnywhere, Category = "Suspension Settings")
	float SpringRate = 250.0f;

	/** The Chassis preload length > 0 */
	UPROPERTY(EditAnywhere, Category = "Suspension Settings")
	float SpringPreLoadLength;
};

USTRUCT(BlueprintType)
struct FTyreclass
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName WheelBoneName;

	UPROPERTY(EditAnywhere)
	FVector WheelLocationOffset;

	UPROPERTY(EditAnywhere)
	FWheelSettings WheelSettings;

	UPROPERTY(EditAnywhere)
	FSuspensionSettings SuspensionSettings;
};

/**
 * Dynamic performance parameters of aircraft
 */
USTRUCT(BlueprintType)
struct FAirplaneThrusterSettings
{
	GENERATED_BODY()

	/** Relative location */
	UPROPERTY(EditAnywhere, Category = "Thruster Settings")
	FVector EngineLocation;

	/** Maximum normal engine thrust Unit Kg */
	UPROPERTY(EditAnywhere, Category = "Thruster Settings")
	float MaxNormalThrust;

	/** Maximum engine thrust Unit Kg */
	UPROPERTY(EditAnywhere, Category = "Thruster Settings")
	float MaxExtraThrust;

	UPROPERTY(EditAnywhere, Category = "Thruster Settings")
	FVector ThrustAixs = FVector(-1.0f, 0.0f, 0.0f);
};

USTRUCT()
struct FWheelCacheVaribles
{
	GENERATED_BODY()

	bool bIsHit;
	FVector LastFrameImpactLocation;
	FVector ImpactLocation;
	float LastFrameDistance;
	float Distance;
	FVector Normal;
	UPhysicalMaterial* SurfaceMatrial;

	float CurrentWheelTurnRate;
};

USTRUCT(BlueprintType)
struct FWheelAnimVaribles
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Wheel")
	float SuspensionDisplacement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Wheel")
	FRotator WheelRotation;
};

USTRUCT(BlueprintType)
struct FAerosufaceAnimVaribles
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Aerosurface")
	float RotDegree;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AERODYNAMICPHYSICS_API UAeroPhysicsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAeroPhysicsComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 *  Airplane's configurations
	 */
	// Jet's empty weight, the unit is kg
	 UPROPERTY(EditAnywhere, Category = "Configs")
	 float EmptyWeight;

	 // The center of mass of aircraft's body, base in local location
	 UPROPERTY(EditAnywhere, Category = "Configs")
	 FVector CenterOfMass;

	 // Tyres Settings
	 UPROPERTY(EditAnywhere, Category = "Configs")
	 TArray<FTyreclass> Tyres;

	 // Thruster Settings
	 UPROPERTY(EditAnywhere, Category = "Configs")
	 TArray<FAirplaneThrusterSettings> ThrusterSettings;

	 // Aerodynamic Surface Config
	 UPROPERTY(EditAnywhere, Category = "Configs")
	 TArray<FAeroSurface> AerodynamicSurfaceSettings;

	 /** Axis Value between 0 - 1 */
	UFUNCTION(BlueprintCallable)
	void SetWheelsBrake(float AxisValue);

	/** Axis Value between -1 - 1 */
	UFUNCTION(BlueprintCallable)
	void SetSteeringWheels(float AxisValue);

	UFUNCTION(BlueprintCallable)
	void SetWheelsRetreated(bool bIsRetreated);

	/** Axis Value between -1 - 1 will add to throttle*/
	UFUNCTION(BlueprintCallable)
	void SetAddThruster(float AxisValue);

	/** Axis Value between -1 - 1 Pitch Control */
	UFUNCTION(BlueprintCallable)
	void SetAeroPitchControl(float AxisValue);

	/** Axis Value between -1 - 1 Roll Control */
	UFUNCTION(BlueprintCallable)
	void SetAeroRollControl(float AxisValue);

	/** Axis Value between -1 - 1 Yaw Control */
	UFUNCTION(BlueprintCallable)
	void SetAeroYawControl(float AxisValue);

	/** Axis Value between -1 - 1 Flap Control */
	UFUNCTION(BlueprintCallable)
	void SetAeroFlapControl(float AxisValue);

protected:
	virtual void BeginPlay() override;

	void InitializeArray();
	void InitializeAnimationInstance();

	/** Airplane physics Calculation */
	void AeroPhysicsTick(float DeltaTime);

	void AddForceToMesh();

	void DebugTick(float DeltaTime);
	void AddDebugMessageOnScreen(const float DisplayTime, const FColor Color, const FString DiplayString);

private:
	class AAirplane* Airplane;
	class USkeletalMeshComponent* Mesh;
	class UAirplaneAnimInstance* MeshAnimInstance;

	float PhysicsDT = 60.0f;
	void AeroPhysicsFun();

	/**
	 * Airplane State Calculation
	 */
	void AeroParametersCalculation(float DeltaTime);

	float CalculateCurrentGForce(float DeltaTime);

	FVector MeshVelocity;
	FVector LastFrameMeshVelocity;
	
	FVector MeshAcceleration;

	FVector MeshAngularVelocityInRadians;

	UPROPERTY(Replicated)
	float GroundSpeed;

	UPROPERTY(Replicated)
	float GForce = 0.0f;

	UPROPERTY(Replicated)
	float AngleOfAttack = 0.0f;

	/**
	  * Wheels Calculation
	  */
	void WheelsForceCalculation(float DeltaTime);
	void WheelsRaycastAndVariablesCache();

	TArray<FWheelCacheVaribles> WheelCalculationVariblesCache;
	TArray<FVector> WheelsForcesToAdd;

	UPROPERTY(EditAnywhere, Category = "Debug Parameters")
	float SuspensionStaticThreshold = 0.05; 

	UPROPERTY(EditAnywhere, Category = "Debug Parameters")
	float WheelStaticThreshold = 0.05; 

	UPROPERTY(EditAnywhere, Category = "Debug Parameters")
	float WheelTurnFrictionRatio = 100; 

	// extra length used for wheel Raycast
	float WheelRayOffset = 100.0f;

	float TargetWheelTurnRate = 0.0f;

	float BrakeForceRatio = 0.0f;

	UPROPERTY(Replicated)
	bool bIsWheelsRetreated = false;

	UPROPERTY(Replicated)
	float WheelRetreatedRatio = 0.0f;

	/**
	 * Thruster Force Calculation
	 */
	void ThrusterForceCalculation(float DeltaTime);

	UPROPERTY(Replicated)
	float CurrentThrusterRatio;

	UPROPERTY(EditAnywhere, Category = "Debug Parameters")
	float ThrusterRatioAddPerSecond = 0.2f; 

	TArray<float> CurrentThrusters;

	TArray<FVector> ThrusterForcesToAdd;

	/**
	 * Aerodynamic Force Calculation
	 */
	void AerodynamicFroceCalculation(float DeltaTime);
	
	float CalculateRotDegree(float ControlAxis, float X, float Y);

	TArray<FBiVector> AeroSufaceForcesAndTorques;
	FBiVector AeroSufaceTotalForceAndTorque;

	float PitchControl = 0.0f;
	float RollControl = 0.0f;
	float YawControl = 0.0f;
	float FlapControl = 0.0f;

	float TargetPitchControl = 0.0f;
	float TargetRollControl = 0.0f;
	float TargetYawControl = 0.0f;
	float TargetFlapControl = 0.0f;

	float PitchInput = 0.0f;
	float RollInput = 0.0f;
	float YawInput = 0.0f;
	float FlapInput = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Debug Parameters")
	bool bShowAeroSufaceDubugBox = false;

	UPROPERTY(EditAnywhere, Category = "AeroControl Parameters")
	float PitchControlInterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "AeroControl Parameters")
	float PitchControlLimitRatio = 300.0f;

	UPROPERTY(EditAnywhere, Category = "AeroControl Parameters")
	float RollControlLimitRatio = 20.0f;

	/**
	 * Flying Control System
	 */
	void CalculateFlyControl(float DeltaTime);

	void InterpAeroControl(float DeltaTime);

public:	
	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<FWheelAnimVaribles> WheelAnimVaribles;

	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<FAerosufaceAnimVaribles> AerosufaceAnimVaribles;

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetIsWheelsRetreated() const { return bIsWheelsRetreated; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetCurrentThrusterRatio() const { return CurrentThrusterRatio; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetCurrentGroundSpeed() const { return GroundSpeed; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetCurrentGForce() const { return GForce; }
		
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetCurrentWheelRetreatedRatio() const { return WheelRetreatedRatio; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetCurrentAngleofAttack() const { return AngleOfAttack; }
};
