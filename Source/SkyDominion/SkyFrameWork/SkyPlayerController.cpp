// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyPlayerController.h"
#include "SkyDominion/Pawn/Fighter.h"
#include "F35SoundSystem/Sounds_F35.h"
#include "Camera/CameraComponent.h"

void ASkyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}

void ASkyPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	AFighter* fighter = Cast<AFighter>(aPawn);

	if (fighter)
	{
		APlayerController* PContorller = GetWorld()->GetFirstPlayerController();

		if (IsLocalPlayerController())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Change Cockpit Sound!"));
			fighter->GetSoundComponent()->SwitchCockpitSnd();
		}
		else
		{
			if (GetWorld() && GetWorld()->GetFirstPlayerController())
			{
				AFighter* TargetFighter = Cast<AFighter>(GetWorld()->GetFirstPlayerController()->GetPawn());
				if (TargetFighter)
				{
					fighter->GetSoundComponent()->ChangeAudioListener(TargetFighter->GetMainCamera());
					//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString("Set Audio to first controller Success!"));
				}
			}
		}
	}
}
