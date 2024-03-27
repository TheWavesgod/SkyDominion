// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyPlayerController.h"
#include "SkyDominion/Pawn/Fighter.h"
//#include "F35SoundSystem/Sounds_F35.h"
#include "Camera/CameraComponent.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"
#include "SkyDominion/HUD/SkyDominionHUD.h"
#include "SkyDominion/GameMode/GM_SkyDominion.h"
#include "SkyDominion/Pawn/SkySpectatorPawn.h"

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
		SetAudioListenerOverride(GetSpectatorPawn()->FindComponentByClass<USceneComponent>(), FVector(0), FRotator(0));
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

void ASkyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("Pause"), EInputEvent::IE_Pressed, this, &ThisClass::PauseBtnPressed);

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

void ASkyPlayerController::PauseBtnPressed()
{
	ASkyDominionHUD* SkyDominionHUD = GetHUD<ASkyDominionHUD>();
	if (!SkyDominionHUD) return;

	SkyDominionHUD->SetPlayerOverlayVisibility(false);
	SkyDominionHUD->RemoveSpectatorOverlay();
	SkyDominionHUD->AddPauseMenu();

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);
	bShowMouseCursor = true;
}

void ASkyPlayerController::ResumeGame()
{
	ASkyDominionHUD* SkyDominionHUD = GetHUD<ASkyDominionHUD>();
	if (!SkyDominionHUD) return;

	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
	bShowMouseCursor = false;

	if (GetStateName() == NAME_Playing)
		SkyDominionHUD->SetPlayerOverlayVisibility(true);
	else
		SkyDominionHUD->AddSpectatorOverlay();

	SkyDominionHUD->RemovePauseMenu();
}