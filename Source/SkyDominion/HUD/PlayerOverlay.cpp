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
	Super::NativeConstruct();
}

void UPlayerOverlay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateParameter();
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
