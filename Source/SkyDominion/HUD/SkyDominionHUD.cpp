// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyDominionHUD.h"
#include "PlayerOverlay.h"
#include "SkyDominion/Pawn/Fighter.h"

void ASkyDominionHUD::BeginPlay()
{
	Super::BeginPlay();

	AddPlayerOverlay();
}

void ASkyDominionHUD::AddPlayerOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && PlayerOverlayClass)
	{
		PlayerOverlay = CreateWidget<UPlayerOverlay>(PlayerController, PlayerOverlayClass);
		PlayerOverlay->AddToViewport();

		APawn* controledPawn = PlayerController->GetPawn();
		AFighter* fighter = Cast<AFighter>(controledPawn);
		if (fighter)
		{
			PlayerOverlay->fighter = fighter;
		}
	}
}

void ASkyDominionHUD::DrawHUD()
{
	Super::DrawHUD();
}
