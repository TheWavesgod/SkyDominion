// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GM_SkyDominion.generated.h"

/**
 * 
 */
UCLASS()
class SKYDOMINION_API AGM_SkyDominion : public AGameMode
{
	GENERATED_BODY()

public:
	//virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void StartPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	void FighterDestroyed(class AFighter* DestroyedFighter, class ASkyPlayerController* VictimController, class ASkyPlayerController* AttackerController);

	void SwitchToSpectator(AFighter* DestroyedFighter, ASkyPlayerController* SpecController);

	void RequestRespawn(ASkyPlayerController* TargetController);

	void StartRoundTimer(int RoundTime);

protected:
	FTimerHandle CountdownTimerHandle;
	void CountdownTick();
	int LeftRoundTime;
};
