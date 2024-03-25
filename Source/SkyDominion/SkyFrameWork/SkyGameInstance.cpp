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
				TSharedPtr<const FUniqueNetId> PlayerID = PlayerState->GetUniqueId().GetUniqueNetId();

				if (!PlayerID.IsValid()) continue;

				FPlayersInfo Info;
				
				Info.bInRedTeam = PlayerState->bInRedTeam;
				Info.TeamIndex = PlayerState->TeamIndex;
				Info.ChooseJet = static_cast<int>(PlayerState->ChoosedFighterType);

				PlayersInfoList.Add(PlayerID, Info);
			}
		}
	}
}
