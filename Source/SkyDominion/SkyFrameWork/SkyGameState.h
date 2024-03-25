// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SkyGameState.generated.h"

/**
 * 
 */
UCLASS()
class SKYDOMINION_API ASkyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	void AddScore_RedTeam();

	void AddScore_BlueTeam();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated)
	int RedTeamScore = 0;

	UPROPERTY(Replicated)
	int BlueTeamScore = 0;

	UPROPERTY(Replicated)
	int RoundTime = 0;

public:
	FORCEINLINE int GetRedTeamScore() const { return RedTeamScore; }
	FORCEINLINE int GetBlueTeamScore() const { return BlueTeamScore; }
	FORCEINLINE int GetRoundTime() const { return RoundTime; }
	void SetRoundTime(int val) { RoundTime = val; }
};

