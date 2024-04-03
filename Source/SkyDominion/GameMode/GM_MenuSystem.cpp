// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_MenuSystem.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"
#include "GameFramework/GameState.h"
#include "SkyDominion/SkyFrameWork/SkyGameInstance.h"
#include "SkyDominion/HUD/LobbyMenu.h"

void AGM_MenuSystem::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ASkyPlayerState* NewPlayerState = NewPlayer->GetPlayerState<ASkyPlayerState>();
	if (NewPlayerState)
	{
		if (GameState->PlayerArray.Num() % 2 == 0)
		{
			NewPlayerState->bInRedTeam = true;
		}
		if ((GameState->PlayerArray.Num()) / 2 != 0)
		{
			NewPlayerState->TeamIndex = 1;
		}
	}

	if (GetGameInstance<USkyGameInstance>()->LobbyMenu)
	{
		GetGameInstance<USkyGameInstance>()->LobbyMenu->UpdatePlayersList();
	}
}

void AGM_MenuSystem::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	for (int i = 0; i < GameState->PlayerArray.Num(); ++i)
	{
		switch (i)
		{
		case 0:
			GameState->PlayerArray[i]->GetPlayerController()->GetPlayerState<ASkyPlayerState>()->bInRedTeam = true;
			GameState->PlayerArray[i]->GetPlayerController()->GetPlayerState<ASkyPlayerState>()->TeamIndex = 0;
			break;
		case 1:
			GameState->PlayerArray[i]->GetPlayerController()->GetPlayerState<ASkyPlayerState>()->bInRedTeam = false;
			GameState->PlayerArray[i]->GetPlayerController()->GetPlayerState<ASkyPlayerState>()->TeamIndex = 0;
			break;
		case 2:
			GameState->PlayerArray[i]->GetPlayerController()->GetPlayerState<ASkyPlayerState>()->bInRedTeam = true;
			GameState->PlayerArray[i]->GetPlayerController()->GetPlayerState<ASkyPlayerState>()->TeamIndex = 1;
			break;
		case 3:
			GameState->PlayerArray[i]->GetPlayerController()->GetPlayerState<ASkyPlayerState>()->bInRedTeam = false;
			GameState->PlayerArray[i]->GetPlayerController()->GetPlayerState<ASkyPlayerState>()->TeamIndex = 1;
			break;
		}
	}

	if (GetGameInstance<USkyGameInstance>()->LobbyMenu)
	{
		GetGameInstance<USkyGameInstance>()->LobbyMenu->UpdatePlayersList();
	}
}
