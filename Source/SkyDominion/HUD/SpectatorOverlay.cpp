// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectatorOverlay.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"
#include "Components/TextBlock.h"

void USpectatorOverlay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateInfo(InDeltaTime);
}

void USpectatorOverlay::UpdateInfo(float InDeltaTime)
{
	/** Player State Info */
	SkyPlayerState = SkyPlayerState == nullptr ? GetOwningLocalPlayer()->GetPlayerController(GetWorld())->GetPlayerState<ASkyPlayerState>() : SkyPlayerState;
	if (!SkyPlayerState) return;

	Kills = SkyPlayerState->GetScore();

	Deaths = SkyPlayerState->GetDefeat();

	RespawnCoolingDownTimer = FMath::Clamp(RespawnCoolingDownTimer - InDeltaTime, 0.0f, RespawnCoolingDownTime);
	RespawnCD = FMath::CeilToInt(RespawnCoolingDownTimer);
}

void USpectatorOverlay::Init()
{
	RespawnCoolingDownTimer = RespawnCoolingDownTime;
	TextBlock_RespawnHint->SetVisibility(ESlateVisibility::Hidden);
}
