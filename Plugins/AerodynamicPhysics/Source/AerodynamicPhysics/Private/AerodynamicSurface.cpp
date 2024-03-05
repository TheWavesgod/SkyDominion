// Fill out your copyright notice in the Description page of Project Settings.


#include "AerodynamicSurface.h"

void FAeroSurface::SetFlapAngle(float angle)
{
	FlapAngle = FMath::Clamp(FMath::DegreesToRadians(angle), FMath::DegreesToRadians(-50.0f), FMath::DegreesToRadians(50.0f));
}

void FAeroSurface::SetFullMoveAngle(float Angle)
{
	//FullControlRotate = FQuat(FVector(0.0f, 1.0f, 0.0f), FMath::DegreesToRadians(Angle));
	FullControlRotate = FQuat(FRotator(Angle, 0.0f, 0.0f));
}

FQuat FAeroSurface::GetCurrentRelativeRotation() const
{
	return RelativeRotation.Quaternion() * FullControlRotate;
}

FBiVector FAeroSurface::CalculateAerodynamicForces(FVector LocalAirVelocity, float AirDensity)
{
	// Accounting for aspect ratio effect on lift coefficient.
	float AspectRatioEffectivness = Config.AspectRatio / (Config.AspectRatio + 2 * (Config.AspectRatio + 4) / (Config.AspectRatio + 2));
	float CorrectedLiftSlope = Config.LiftSlope * AspectRatioEffectivness;

	// Calculating flap deflection influence on zero lift angle of attack and angles at which stall happens.
	float theta = FMath::Acos(2 * Config.FlapFraction - 1);
	float flapEffectivness = 1 - (theta - FMath::Sin(theta)) / PI;
	float deltaLift = CorrectedLiftSlope * flapEffectivness * FlapEffectivnessCorrection(FlapAngle) * FlapAngle;

	float zeroLiftAoaBase = FMath::DegreesToRadians(Config.ZeroLiftAoA);
	float zeroLiftAoA = zeroLiftAoaBase - deltaLift / CorrectedLiftSlope;

	float stallAngleHighBase = FMath::DegreesToRadians(Config.StallAngleHigh);
	float stallAngleLowBase = FMath::DegreesToRadians(Config.StallAngleLow);

	float clMaxHigh = CorrectedLiftSlope * (stallAngleHighBase - zeroLiftAoaBase) + deltaLift * LiftCoefficientMaxFraction(Config.FlapFraction);
	float clMaxLow = CorrectedLiftSlope * (stallAngleLowBase - zeroLiftAoaBase) + deltaLift * LiftCoefficientMaxFraction(Config.FlapFraction);

	float stallAngleHigh = zeroLiftAoA + clMaxHigh / CorrectedLiftSlope;
	float stallAngleLow = zeroLiftAoA + clMaxLow / CorrectedLiftSlope;

	// Calculating air velocity relative to the surface's coordinate system.
	// Y component of the velocity is discarded. 
	FQuat CurrentRelativeRotation = RelativeRotation.Quaternion() * FullControlRotate;
	FVector airVelocity = CurrentRelativeRotation.UnrotateVector(LocalAirVelocity);
	airVelocity = FVector(airVelocity.X, 0.0f, airVelocity.Z);
	FVector dragDirection = CurrentRelativeRotation.RotateVector(airVelocity.GetSafeNormal());
	FVector liftDirection = CurrentRelativeRotation.GetRightVector().Cross(dragDirection);

	float area = Config.Chord * Config.Span;
	float dynamicPressure = 0.5f * AirDensity * FMath::Square(airVelocity.Size() * 0.01f);
	float angleOfAttack = FMath::Atan2(airVelocity.Z, -airVelocity.X);

	FVector aerodynamicCoefficients = CalculateCoefficients(angleOfAttack, CorrectedLiftSlope, zeroLiftAoA, stallAngleHigh, stallAngleLow);

	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, SurfaceName.ToString() + FString::Printf(TEXT(" AoA: %f, Cl: %f"), FMath::RadiansToDegrees(angleOfAttack), aerodynamicCoefficients.X)); }

	FVector lift = liftDirection * aerodynamicCoefficients.X * dynamicPressure * area;
	FVector drag = dragDirection * aerodynamicCoefficients.Y * dynamicPressure * area;
	FVector torque = CurrentRelativeRotation.GetRightVector() * aerodynamicCoefficients.Z * dynamicPressure * area * Config.Chord;
	FVector LiftAndDragTorque = RelativePosition.Cross(lift + drag) * 0.01f;

	return FBiVector(lift + drag, torque + LiftAndDragTorque);
	//return FBiVector(CurrentRelativeRotation.GetForwardVector(), CurrentRelativeRotation.GetUpVector());
	//return FBiVector(lift, drag);
}

FVector FAeroSurface::CalculateCoefficients(float angleOfAttack, float correctedLiftSlope, float zeroLiftAoA, float stallAngleHigh, float stallAngleLow)
{
	FVector aerodynamicCoefficients;

	// Low angles of attack mode and stall mode curves are stitched together by a line segment. 
	float paddingAngleHigh = FMath::DegreesToRadians(FMath::Lerp(15.0f, 5.0f, (FMath::RadiansToDegrees(FlapAngle) + 50) / 100));
	float paddingAngleLow = FMath::DegreesToRadians(FMath::Lerp(15.0f, 5.0f, (-FMath::RadiansToDegrees(FlapAngle) + 50) / 100));
	float paddedStallAngleHigh = stallAngleHigh + paddingAngleHigh;
	float paddedStallAngleLow = stallAngleLow - paddingAngleLow;

	if (angleOfAttack < stallAngleHigh && angleOfAttack > stallAngleLow)
	{
		// Low angle of attack mode.
		aerodynamicCoefficients = CalculateCoefficientsAtLowAoA(angleOfAttack, correctedLiftSlope, zeroLiftAoA);
	}
	else
	{
		if (angleOfAttack > paddedStallAngleHigh || angleOfAttack < paddedStallAngleLow)
		{
			// Stall mode.
			aerodynamicCoefficients = CalculateCoefficientsAtStall(angleOfAttack, correctedLiftSlope, zeroLiftAoA, stallAngleHigh, stallAngleLow);
		}
		else
		{
			// Linear stitching in-between stall and low angles of attack modes.
			FVector aerodynamicCoefficientsLow;
			FVector aerodynamicCoefficientsStall;
			float lerpParam;

			if (angleOfAttack > stallAngleHigh)
			{
				aerodynamicCoefficientsLow = CalculateCoefficientsAtLowAoA(stallAngleHigh, correctedLiftSlope, zeroLiftAoA);
				aerodynamicCoefficientsStall = CalculateCoefficientsAtStall(paddedStallAngleHigh, correctedLiftSlope, zeroLiftAoA, stallAngleHigh, stallAngleLow);
				lerpParam = (angleOfAttack - stallAngleHigh) / (paddedStallAngleHigh - stallAngleHigh);
			}
			else
			{
				aerodynamicCoefficientsLow = CalculateCoefficientsAtLowAoA(stallAngleLow, correctedLiftSlope, zeroLiftAoA);
				aerodynamicCoefficientsStall = CalculateCoefficientsAtStall(paddedStallAngleLow, correctedLiftSlope, zeroLiftAoA, stallAngleHigh, stallAngleLow);
				lerpParam = (angleOfAttack - stallAngleLow) / (paddedStallAngleLow - stallAngleLow);
			}
			aerodynamicCoefficients = FMath::Lerp(aerodynamicCoefficientsLow, aerodynamicCoefficientsStall, lerpParam);
		}
	}
	return aerodynamicCoefficients;
}

FVector FAeroSurface::CalculateCoefficientsAtLowAoA(float angleOfAttack, float correctedLiftSlope, float zeroLiftAoA)
{
	float liftCoefficient = correctedLiftSlope * (angleOfAttack - zeroLiftAoA);
	float inducedAngle = liftCoefficient / (PI * Config.AspectRatio);
	float effectiveAngle = angleOfAttack - zeroLiftAoA - inducedAngle;

	float tangentialCoefficient = Config.SkinFriction * FMath::Cos(effectiveAngle);

	float normalCoefficient = (liftCoefficient + FMath::Sin(effectiveAngle) * tangentialCoefficient) / FMath::Cos(effectiveAngle);
	float dragCoefficient = normalCoefficient * FMath::Sin(effectiveAngle) + tangentialCoefficient * FMath::Cos(effectiveAngle);
	float torqueCoefficient = -normalCoefficient * TorqCoefficientProportion(effectiveAngle);

	return FVector(liftCoefficient, dragCoefficient, torqueCoefficient);
}

FVector FAeroSurface::CalculateCoefficientsAtStall(float angleOfAttack, float correctedLiftSlope, float zeroLiftAoA, float stallAngleHigh, float stallAngleLow)
{
	float liftCoefficientLowAoA;
	if (angleOfAttack > stallAngleHigh)
	{
		liftCoefficientLowAoA = correctedLiftSlope * (stallAngleHigh - zeroLiftAoA);
	}
	else
	{
		liftCoefficientLowAoA = correctedLiftSlope * (stallAngleLow - zeroLiftAoA);
	}
	float inducedAngle = liftCoefficientLowAoA / (PI * Config.AspectRatio);

	float lerpParam;
	if (angleOfAttack > stallAngleHigh)
	{
		lerpParam = (PI / 2 - FMath::Clamp(angleOfAttack, -PI / 2, PI / 2)) / (PI / 2 - stallAngleHigh);
	}
	else
	{
		lerpParam = (-PI / 2 - FMath::Clamp(angleOfAttack, -PI / 2, PI / 2)) / (-PI / 2 - stallAngleLow);
	}
	inducedAngle = FMath::Lerp(0, inducedAngle, lerpParam);
	float effectiveAngle = angleOfAttack - zeroLiftAoA - inducedAngle;

	float normalCoefficient = FrictionAt90Degrees(FlapAngle) * FMath::Sin(effectiveAngle) * (1 / (0.56f + 0.44f * FMath::Abs(FMath::Sin(effectiveAngle))) - 0.41f * (1 - FMath::Exp(-17 / Config.AspectRatio)));
	float tangentialCoefficient = 0.5f * Config.SkinFriction * FMath::Cos(effectiveAngle);

	float liftCoefficient = normalCoefficient * FMath::Cos(effectiveAngle) - tangentialCoefficient * FMath::Sin(effectiveAngle);
	float dragCoefficient = normalCoefficient * FMath::Sin(effectiveAngle) + tangentialCoefficient * FMath::Cos(effectiveAngle);
	float torqueCoefficient = -normalCoefficient * TorqCoefficientProportion(effectiveAngle);

	return FVector(liftCoefficient, dragCoefficient, torqueCoefficient);
}

float FAeroSurface::FlapEffectivnessCorrection(float flapAngle)
{
	return FMath::Lerp(0.8f, 0.4f, (FMath::RadiansToDegrees(FMath::Abs(flapAngle)) - 10) / 50);
}

float FAeroSurface::LiftCoefficientMaxFraction(float flapFraction)
{
	return FMath::Clamp(1 - 0.5f * (flapFraction - 0.1f) / 0.3f, 0.0f, 1.0f);
}

float FAeroSurface::TorqCoefficientProportion(float effectiveAngle)
{
	return 0.25f - 0.175f * (1 - 2 * FMath::Abs(effectiveAngle) / PI);
}

float FAeroSurface::FrictionAt90Degrees(float flapAngle)
{
	return 1.98f - 4.26e-2f * flapAngle * flapAngle + 2.1e-1f * flapAngle;
}
