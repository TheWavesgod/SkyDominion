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
	virtual void StartPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
