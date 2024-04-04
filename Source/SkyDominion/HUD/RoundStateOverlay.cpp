// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundStateOverlay.h"
#include "SkyDominion/SkyFrameWork/SkyGameState.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"
#include "Components/TextBlock.h"

void URoundStateOverlay::NativeConstruct()
{
	UWorld* World = GetWorld();
	if (World)
	{
		FTimerHandle UpdateListHandle;
		World->GetTimerManager().SetTimer(UpdateListHandle, this, &ThisClass::UpdatePlayerList, 0.1f, true);
	}
}

void URoundStateOverlay::UpdatePlayerList()
{
	if (!bCanUpdate) return;

	SkyGameState = SkyGameState == nullptr ? GetWorld()->GetGameState<ASkyGameState>() : SkyGameState;

	if (!SkyGameState) return;

	ASkyPlayerState* Red1 = nullptr;
	ASkyPlayerState* Red2 = nullptr;
	ASkyPlayerState* Blue1 = nullptr;
	ASkyPlayerState* Blue2 = nullptr;

	for (auto i : SkyGameState->PlayerArray)
	{
		ASkyPlayerState* SkyPState = Cast<ASkyPlayerState>(i);
		if (!SkyPState) continue;

		if (SkyPState->bInRedTeam)
		{
			if (SkyPState->TeamIndex == 0)
			{
				Red1 = SkyPState;
			}
			else
			{
				Red2 = SkyPState;
			}
		}
		else
		{
			if (SkyPState->TeamIndex == 0)
			{
				Blue1 = SkyPState;
			}
			else
			{
				Blue2 = SkyPState;
			}
		}
	}

	if (Red2)
	{
		if (Red1->GetScore() < Red2->GetScore())
		{
			WritePlayerState(Red2, 0);
			WritePlayerState(Red1, 1);
		}
		else
		{
			WritePlayerState(Red1, 0);
			WritePlayerState(Red2, 1);
		}
	}
	else
	{
		WritePlayerState(Red1, 0);
		WritePlayerState(Red2, 1);
	}

	if (Blue2)
	{
		if (Blue1->GetScore() < Blue2->GetScore())
		{
			WritePlayerState(Blue2, 2);
			WritePlayerState(Blue1, 3);
		}
		else
		{
			WritePlayerState(Blue1, 2);
			WritePlayerState(Blue2, 3);
		}
	}
	else
	{
		WritePlayerState(Blue1, 2);
		WritePlayerState(Blue2, 3);
	}
}

void URoundStateOverlay::WritePlayerState(ASkyPlayerState* targetState, int32 index)
{
	if (index > 3) return;

	UTextBlock* Name = nullptr;
	UTextBlock* Kill = nullptr;
	UTextBlock* Dead = nullptr;

	switch (index)
	{
	case 0:
		Name = Text_Name_Red1;
		Kill = Text_Kill_Red1;
		Dead = Text_Dead_Red1;
		break;

	case 1:
		Name = Text_Name_Red2;
		Kill = Text_Kill_Red2;
		Dead = Text_Dead_Red2;
		break;

	case 2:
		Name = Text_Name_Blue1;
		Kill = Text_Kill_Blue1;
		Dead = Text_Dead_Blue1;
		break;

	case 3:
		Name = Text_Name_Blue2;
		Kill = Text_Kill_Blue2;
		Dead = Text_Dead_Blue2;
		break;
	}

	if (!Name || !Kill || !Dead) return;

	if (targetState == nullptr)
	{
		SetTargetText(Name, FString(" "));
		SetTargetText(Kill, FString(" "));
		SetTargetText(Dead, FString(" "));
		return;
	}

	SetTargetText(Name, targetState->GetPlayerName());
	SetTargetText(Kill, FString::FromInt(targetState->GetScore()));
	SetTargetText(Dead, FString::FromInt(targetState->GetDefeat()));
}

void URoundStateOverlay::SetTargetText(UTextBlock* target, const FString& Val)
{
	if (!target) return;

	FText ValText = FText::FromString(Val);

	if (ValText.EqualTo(target->GetText()))return;

	target->SetText(ValText);
}

