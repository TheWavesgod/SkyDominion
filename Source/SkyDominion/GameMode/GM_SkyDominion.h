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

	//virtual void StartPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
};
