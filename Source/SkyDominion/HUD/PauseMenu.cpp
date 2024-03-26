// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenu.h"
#include "Components/Button.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerController.h"
#include "Kismet/GameplayStatics.h"

bool UPauseMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (Bttn_ResumeGame)
	{
		Bttn_ResumeGame->OnClicked.AddDynamic(this, &ThisClass::ResumeGameBtnClicked);
	}

	if (Bttn_MainMenu)
	{
		Bttn_MainMenu->OnClicked.AddDynamic(this, &ThisClass::MainMenuBtnClicked);
	}

	return true;
}

void UPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPauseMenu::ResumeGameBtnClicked()
{
	ASkyPlayerController* SkyPlayerController = Cast<ASkyPlayerController>(GetOwningLocalPlayer()->GetPlayerController(GetWorld()));
	if (!SkyPlayerController) return;

	SkyPlayerController->ResumeGame();
}

void UPauseMenu::MainMenuBtnClicked()
{
	UGameplayStatics::OpenLevel(this, TEXT("MainMenu"));
}
