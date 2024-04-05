// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundOverOverlay.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "SkyDominion/SkyFrameWork/SkyGameState.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerController.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"

void URoundOverOverlay::InitRoundOverOverlay()
{
	UWorld* World = GetWorld();
	if (!World) return;

	ASkyGameState* SkyGameState = World->GetGameState<ASkyGameState>();
	if (!SkyGameState) return;

	int RedTeamScore = SkyGameState->GetRedTeamScore();
	int BlueTeamScore = SkyGameState->GetBlueTeamScore();

	Text_RedScore->SetText(FText::FromString(FString::FromInt(RedTeamScore)));
	Text_BlueScore->SetText(FText::FromString(FString::FromInt(BlueTeamScore)));

	ASkyPlayerController* PlayerController = Cast<ASkyPlayerController>(World->GetFirstLocalPlayerFromController()->GetPlayerController(World));
	if (!PlayerController) return;

	ASkyPlayerState* SkyPlayerState = PlayerController->GetPlayerState<ASkyPlayerState>();
	if (!SkyPlayerState) return;

	if (RedTeamScore == BlueTeamScore)
	{
		Text_Result->SetText(FText::FromString("Tie"));
	}
	else 
	{
		if ((RedTeamScore > BlueTeamScore && SkyPlayerState->bInRedTeam) || (RedTeamScore < BlueTeamScore && !SkyPlayerState->bInRedTeam))
		{
			Text_Result->SetText(FText::FromString("You Win"));
		}
		else
		{
			Text_Result->SetText(FText::FromString("You Loss"));
		}
	}

	if (!PlayerController->HasAuthority())
	{
		Bttn_Lobby->SetVisibility(ESlateVisibility::Collapsed);
	}
}

bool URoundOverOverlay::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (Bttn_Lobby)
	{
		Bttn_Lobby->OnClicked.AddDynamic(this, &ThisClass::OnLobbyBttnClicked);
	}

	return true;
}

void URoundOverOverlay::OnLobbyBttnClicked()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel(TEXT("/Game/Maps/MultiplayerLobby?listen"));
	}
}
