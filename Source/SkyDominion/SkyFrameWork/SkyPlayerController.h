// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SkyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SKYDOMINION_API ASkyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASkyPlayerController();

	UFUNCTION(Client, Reliable)
	void SetSpectatorState(bool bActivated);

	UFUNCTION(Server, Reliable)
	void ClientDisconnect();

	UFUNCTION(Client, Reliable)
	void SendKillMessage(class ASkyPlayerState* VictimPlayerState);

	UFUNCTION(Client, Reliable)
	void SendDefeatMessage(ASkyPlayerState* AttackerPlayerState);

	UFUNCTION(Client, Reliable)
	void ShowRoundOverMessage();

	virtual void SetupInputComponent() override;

	void RequestRespawn();

	UFUNCTION()
	void ResumeGame();

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* aPawn) override;

	UPROPERTY()
	class ASkyDominionHUD* SkyHUD;

	UFUNCTION(Server, Reliable)
	void ServerRequestRespawn();

	UFUNCTION(Client, Reliable)
	void AddPlayerOverlay();

	UFUNCTION()
	void PauseBtnPressed();

	UFUNCTION()
	void ShowRoundStateBtnPressed();

	UFUNCTION()
	void ShowRoundStateBtnReleased();
};
