// Fill out your copyright notice in the Description page of Project Settings.


#include "KillMessageOverlay.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void UKillMessageOverlay::Init(bool bIsKillMessage, const FString& PlayerName)
{
	if (bIsKillMessage)
	{
		Box_KillMessage->SetVisibility(ESlateVisibility::Visible);
		Box_DefeatMessage->SetVisibility(ESlateVisibility::Hidden);

		Text_VictimName->SetText(FText::FromString(PlayerName));
	}
	else
	{
		Box_KillMessage->SetVisibility(ESlateVisibility::Hidden);
		Box_DefeatMessage->SetVisibility(ESlateVisibility::Visible);

		Text_AttackName->SetText(FText::FromString(PlayerName));
	}
}
