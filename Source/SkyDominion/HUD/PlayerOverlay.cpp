// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerOverlay.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "AerodynamicPhysics/public/AeroPhysicsComponent.h"

void UPlayerOverlay::NativeConstruct()
{
	FTimerHandle ParameterUpdateHandle;
	float Interval = 1.0f / 60.0f;
	GetWorld()->GetTimerManager().SetTimer(ParameterUpdateHandle, this, &ThisClass::UpdateParameter, Interval, true);
}

void UPlayerOverlay::Tick(FGeometry MyGeometry, float InDeltaTime)
{
	Super::Tick(MyGeometry, InDeltaTime);
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
