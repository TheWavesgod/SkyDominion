// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyPlayerController.h"
#include "SkyDominion/Pawn/Fighter.h"
//#include "F35SoundSystem/Sounds_F35.h"
#include "Camera/CameraComponent.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"

void ASkyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}

void ASkyPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (HasAuthority())
	{
		AFighter* fighter = Cast<AFighter>(aPawn);
		if (fighter)
		{
			fighter->bInRedTeam = GetPlayerState<ASkyPlayerState>()->bInRedTeam;
		}
	}
}
