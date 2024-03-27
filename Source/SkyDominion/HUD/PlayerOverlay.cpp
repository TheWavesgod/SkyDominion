// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerOverlay.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"
#include "AerodynamicPhysics/public/AeroPhysicsComponent.h"
#include "SkyDominion/SkyFrameWork/SkyGameState.h"

//#include "Sound/SoundCue.h"
//#include "Components/AudioComponent.h"

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
	fighter = fighter == nullptr ? GetOwningPlayerPawn<AFighter>() : fighter;
	if (!fighter) return;

	ThrusterRatio = fighter->GetAeroPhysicsComponent()->GetCurrentThrusterRatio();

	bAfterBurnerActivated = ThrusterRatio > fighter->GetAeroPhysicsComponent()->GetAfterBurnerThresholdRatio();

	Altitude = fighter->GetActorLocation().Z / 100.0f;
	if (GroundSpeed > 300.0f && Altitude < 200.0f) { bShowLowAltitudeAlert = true; }
	else { bShowLowAltitudeAlert = false; }

	AngleOfAttack = fighter->GetAeroPhysicsComponent()->GetCurrentAngleofAttack();

	GroundSpeed = fighter->GetAeroPhysicsComponent()->GetCurrentGroundSpeed();

	GForce = fighter->GetAeroPhysicsComponent()->GetCurrentGForce();

	bIsWheelRetreated = fighter->GetAeroPhysicsComponent()->GetIsWheelsRetreated();

	HealthPercent = fighter->GetCurrentHealthPercent();

	AutoCannonBulletAmount = fighter->GetAutoCannonBulletLeft();

	FighterRotation = fighter->GetActorRotation();

	MissileName = fighter->GetCurrentMissileName();

	MissileNum = fighter->GetCurrentMissileNum();

	/** Player State Info */
	SkyPlayerState = SkyPlayerState == nullptr ? GetOwningLocalPlayer()->GetPlayerController(GetWorld())->GetPlayerState<ASkyPlayerState>() : SkyPlayerState;
	if (!SkyPlayerState) return;

	Kill = SkyPlayerState->GetScore();

	Defeat = SkyPlayerState->GetDefeat();

	/** Game State Info */
	SkyGameState = SkyGameState == nullptr ? GetWorld()->GetGameState<ASkyGameState>() : SkyGameState;
	if (!SkyGameState) return;

	Score_RedTeam = SkyGameState->GetRedTeamScore();
	Score_BlueTeam = SkyGameState->GetBlueTeamScore();
	LeftRoundTime = SkyGameState->GetRoundTime();
}

void UPlayerOverlay::ActivateLowAltitudeAlertSound(bool bActivated)
{
	if (!fighter) return;
	
	fighter->ActivateAlertSoundLowAltitude(bActivated);
}
