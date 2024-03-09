// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyGameInstance.h"
#include "GameFramework/GameState.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"

void USkyGameInstance::UpdatePlayersChooseJetList()
{
	PlayersChooseJetList.Empty();
	if (GetWorld() && GetWorld()->GetGameState())
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		for (int i = 0; i < GameState->PlayerArray.Num(); ++i)
		{
			ASkyPlayerState* PlayerState = GameState->PlayerArray[i]->GetOwningController()->GetPlayerState<ASkyPlayerState>();
			if (PlayerState)
			{
				PlayersChooseJetList.Add(int32(PlayerState->ChoosedFighterType));
			}
			else
			{
				PlayersChooseJetList.Add(0);
			}
		}
	}
}
