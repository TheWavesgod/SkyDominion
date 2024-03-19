#include "SkySpectatorPawn.h"
#include "Camera/CameraComponent.h"
#include "SkyDominion/GameMode/GM_SkyDominion.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerController.h"
#include "SkyDominion/HUD/SkyDominionHUD.h"

ASkySpectatorPawn::ASkySpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ASkySpectatorPawn::BeginPlay()
{
	Super::BeginPlay();
	
}


void ASkySpectatorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkySpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("ReqestRespawn"), EInputEvent::IE_Pressed, this, &ThisClass::RespawnRequestPressed);
}

void ASkySpectatorPawn::RespawnRequestPressed()
{
	ASkyPlayerController* OnwingController = Cast<ASkyPlayerController>(Controller);
	if (OnwingController)
	{
		OnwingController->RequestRespawn();
	}
}


