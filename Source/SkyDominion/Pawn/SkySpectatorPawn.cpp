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
	
	CurrentCoolingTimeHandle = RespawnCoolingTime;
}


void ASkySpectatorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentCoolingTimeHandle = FMath::Clamp(CurrentCoolingTimeHandle - DeltaTime, 0.0f, RespawnCoolingTime);
}

void ASkySpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("ReqestRespawn"), EInputEvent::IE_Pressed, this, &ThisClass::RespawnRequestPressed);
}

void ASkySpectatorPawn::RespawnRequestPressed()
{
	if (CurrentCoolingTimeHandle != 0.0f) return;

	ASkyPlayerController* OnwingController = Cast<ASkyPlayerController>(Controller);
	if (OnwingController)
	{
		OnwingController->RequestRespawn();
	}
}


