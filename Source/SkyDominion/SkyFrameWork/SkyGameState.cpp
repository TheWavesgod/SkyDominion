// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyGameState.h"
#include "Net/UnrealNetwork.h"

void ASkyGameState::AddScore_RedTeam()
{
	if (HasAuthority())
	{
		++RedTeamScore;
	}
}

void ASkyGameState::AddScore_BlueTeam()
{
	if (HasAuthority())
	{
		++BlueTeamScore;
	}
}

void ASkyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASkyGameState, RedTeamScore);
	DOREPLIFETIME(ASkyGameState, BlueTeamScore);
	DOREPLIFETIME(ASkyGameState, RoundTime);
}
