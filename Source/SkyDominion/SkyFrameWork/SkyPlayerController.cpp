// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyPlayerController.h"
#include "SkyDominion/Pawn/Fighter.h"
//#include "F35SoundSystem/Sounds_F35.h"
#include "Camera/CameraComponent.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"
#include "SkyDominion/HUD/SkyDominionHUD.h"
#include "SkyDominion/GameMode/GM_SkyDominion.h"

ASkyPlayerController::ASkyPlayerController()
{
	bReplicates = true;
}

void ASkyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);

	SkyHUD = GetHUD<ASkyDominionHUD>();
}

void ASkyPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	AFighter* fighter = Cast<AFighter>(aPawn);
	if (fighter)
	{
		if (HasAuthority())
		{
			fighter->bInRedTeam = GetPlayerState<ASkyPlayerState>()->bInRedTeam;
		}

		AddPlayerOverlay();
	}
}

void ASkyPlayerController::SetSpectatorState_Implementation(bool bActivated)
{
	if (bActivated)
	{
		ChangeState(NAME_Spectating);
	}
	else
	{
		ChangeState(NAME_Playing);
	}
}

void ASkyPlayerController::ClientDisconnect_Implementation()
{
	ConsoleCommand(TEXT("disconnect"));
}

void ASkyPlayerController::RequestRespawn()
{
	ASkyDominionHUD* SkyDominionHUD = GetHUD<ASkyDominionHUD>();
	if (SkyDominionHUD)
	{
		SkyDominionHUD->RemoveSpectatorOverlay();
	}
	ServerRequestRespawn();
}

void ASkyPlayerController::ServerRequestRespawn_Implementation()
{
	AGM_SkyDominion* SkyDominionGM = GetWorld()->GetAuthGameMode<AGM_SkyDominion>();
	if (SkyDominionGM)
	{
		SkyDominionGM->RequestRespawn(this);
	}
}

void ASkyPlayerController::AddPlayerOverlay_Implementation()
{
	ASkyDominionHUD* SkyDominionHUD = GetHUD<ASkyDominionHUD>();
	if (SkyDominionHUD)
	{
		SkyDominionHUD->AddPlayerOverlay();
	}
}