// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SkyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SKYDOMINION_API ASkyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	bool bInRedTeam = true;

	UPROPERTY(Replicated)
	int32 TeamIndex = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
};
