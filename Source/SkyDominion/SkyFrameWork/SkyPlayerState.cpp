// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SkyDominion/SkyFrameWork/SkyGameInstance.h"
#include "SkyDominion/HUD/LobbyMenu.h"

void ASkyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASkyPlayerState, bInRedTeam);
	DOREPLIFETIME(ASkyPlayerState, TeamIndex);
	DOREPLIFETIME(ASkyPlayerState, ChoosedFighterType);
	DOREPLIFETIME(ASkyPlayerState, Defeat);
}

void ASkyPlayerState::AddKill()
{
	if (HasAuthority())
	{
		SetScore(GetScore() + 1);
	}
}

void ASkyPlayerState::AddDefeat()
{
	if (HasAuthority())
	{
		++Defeat;
	}
}

void ASkyPlayerState::ServerChangeChoosedFighterType_Implementation(int32 Index)
{
	ChoosedFighterType = static_cast<EFighterJetType>(Index);

	if (GetGameInstance<USkyGameInstance>()->LobbyMenu)
	{
		GetGameInstance<USkyGameInstance>()->LobbyMenu->UpdatePlayersFighterType();
	}
}
