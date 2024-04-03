// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyGameInstance.h"
#include "GameFramework/GameState.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"

void USkyGameInstance::UpdatePlayersInfoList()
{
	PlayersInfoList.Empty();
	if (GetWorld() && GetWorld()->GetGameState())
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		for (int i = 0; i < GameState->PlayerArray.Num(); ++i)
		{
			ASkyPlayerState* PlayerState = Cast<ASkyPlayerState>(GameState->PlayerArray[i]);
			if (PlayerState)
			{
				FString PlayerID = PlayerState->UniqueId->ToString();

				//UE_LOG(LogTemp, Warning, TEXT("player %d ID: %s"), i, *PlayerID);

				FPlayersInfo Info;
				
				Info.bInRedTeam = PlayerState->bInRedTeam;
				Info.TeamIndex = PlayerState->TeamIndex;
				Info.ChooseJet = static_cast<int>(PlayerState->ChoosedFighterType);

				PlayersInfoList.Add(PlayerID, Info);
			}
		}
	}
}
