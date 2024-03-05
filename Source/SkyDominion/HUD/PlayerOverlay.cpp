// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerOverlay.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "AerodynamicPhysics/public/AeroPhysicsComponent.h"

bool UPlayerOverlay::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	return true;
}

void UPlayerOverlay::NativeConstruct()
{
	FTimerHandle ParameterUpdateHandle;
	float Interval = 0.1;
	GetWorld()->GetTimerManager().SetTimer(ParameterUpdateHandle, this, &ThisClass::UpdateParameter, Interval, true);
}

void UPlayerOverlay::UpdateParameter()
{
	if (!fighter) return;

	ThrusterRatio = fighter->GetAeroPhysicsComponent()->GetCurrentThrusterRatio();

	Altitude = fighter->GetActorLocation().Z / 100.0f;

	AngleOfAttack = fighter->GetAeroPhysicsComponent()->GetCurrentAngleofAttack();

	GroundSpeed = fighter->GetAeroPhysicsComponent()->GetCurrentGroundSpeed();

	GForce = fighter->GetAeroPhysicsComponent()->GetCurrentGForce();
}
