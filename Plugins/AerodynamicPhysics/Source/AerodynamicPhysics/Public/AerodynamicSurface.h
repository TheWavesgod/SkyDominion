// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AerodynamicSurface.generated.h"
/**
 * 
 */
USTRUCT()
struct FBiVector
{
	GENERATED_BODY()

	FBiVector() {
		this->f = FVector::ZeroVector;
		this->t = FVector::ZeroVector;
	}

	FBiVector(FVector force, FVector torque)
	{
		this->f = force;
		this->t = torque;
	}

	FVector f;

	FVector t;
};

USTRUCT(BlueprintType)
struct FAeroSurfaceControlConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	FVector2D ControlAngleMapDegree;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	bool bIsFullMove = false;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	bool bPitchControl = false;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	float PitchControlRatio = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	bool bRollControl = false;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	float RollControlRatio = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	bool bYawControl = false;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	float YawControlRatio = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	bool bFlapControl = false;
};

USTRUCT(BlueprintType)
struct FAeroSurfaceConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	float LiftSlope = 6.28f;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	float SkinFriction = 0.02f;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	float ZeroLiftAoA = 0;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	float StallAngleHigh = 15;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	float StallAngleLow = -15;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	float FlapFraction = 0;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	float Chord = 1;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	float Span = 1;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	bool AutoAspectRatio = true;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	float AspectRatio = 2;

	void OnValidate()
	{
		if (FlapFraction > 0.4f)
			FlapFraction = 0.4f;
		if (FlapFraction < 0)
			FlapFraction = 0;

		if (StallAngleHigh < 0) StallAngleHigh = 0;
		if (StallAngleLow > 0) StallAngleLow = 0;

		if (Chord < 1e-3f)
			Chord = 1e-3f;

		if (AutoAspectRatio)
		{
			AspectRatio = Span / Chord;
		}
	}
};

USTRUCT(BlueprintType)
struct FAeroSurface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Setting")
	FName SurfaceName; 

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Setting")
	FVector RelativePosition;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Setting")
	FRotator RelativeRotation;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Setting")
	FAeroSurfaceControlConfig ControlConfig;

	UPROPERTY(EditAnywhere, Category = "Aerodynamic Surface Config")
	FAeroSurfaceConfig Config;

	void SetFlapAngle(float Angle);
	void SetFullMoveAngle(float Angle);
	FQuat GetCurrentRelativeRotation() const;

	FBiVector CalculateAerodynamicForces(FVector LocalAirVelocity, float AirDensity, FVector CenterOfMass, bool bDebug = false);

private:
	FVector CalculateCoefficients(float angleOfAttack, float correctedLiftSlope, float zeroLiftAoA, float stallAngleHigh, float stallAngleLow);
	FVector CalculateCoefficientsAtLowAoA(float angleOfAttack, float correctedLiftSlope, float zeroLiftAoA);
	FVector CalculateCoefficientsAtStall(float angleOfAttack, float correctedLiftSlope, float zeroLiftAoA, float stallAngleHigh, float stallAngleLow);

	float FlapEffectivnessCorrection(float flapAngle);

	float LiftCoefficientMaxFraction(float flapFraction);

	float TorqCoefficientProportion(float effectiveAngle);

	float FrictionAt90Degrees(float flapAngle);

	float FlapAngle = 0.0f;

	FQuat FullControlRotate = FQuat(FRotator(0.0f, 0.0f, 0.0f));
};


