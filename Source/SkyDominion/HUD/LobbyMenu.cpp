// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerSessions/Public/MultiplayerSessionsSubsystem.h"

bool ULobbyMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (Bttn_MainMenu)
	{
		Bttn_MainMenu->OnClicked.AddDynamic(this, &ThisClass::MainMenuBttnClicked);
	}

	if (Bttn_StartGame)
	{
		Bttn_StartGame->OnClicked.AddDynamic(this, &ThisClass::StartGameBttnClicked);
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
	}

	return true;
}

void ULobbyMenu::NativeConstruct()
{
	SetUserNameTextBlock();
	UpdatePlayerListLocal();

	if (GetWorld()->GetNetMode() == ENetMode::NM_ListenServer)
	{
		Bttn_StartGame->SetIsEnabled(true);
		Text_StarttGameBttn->SetText(FText::FromString("Start Game"));
	}
	else
	{
		Bttn_StartGame->SetIsEnabled(false);
		Text_StarttGameBttn->SetText(FText::FromString("Waiting for Server"));
	}
}

void ULobbyMenu::MainMenuBttnClicked()
{
	UWorld* World = GetWorld();
	if (World)
	{
		/*if (World->GetNetMode() == ENetMode::NM_ListenServer)
		{
			MultiplayerSessionsSubsystem->DestroySession();
		}*/
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void ULobbyMenu::StartGameBttnClicked()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel(TEXT("/Game/Maps/AirCombatTestMap?listen")); 
	}
}

void ULobbyMenu::SetUserNameTextBlock()
{
	if (Text_UserName)
	{
		APlayerState* OwningPlayerState = UGameplayStatics::GetPlayerController(this, 0)->GetPlayerState<APlayerState>();
		if (OwningPlayerState)
		{
			FString PlayerName = OwningPlayerState->GetPlayerName();
			Text_UserName->SetText(FText::FromString(PlayerName));
		}
	}
}

void ULobbyMenu::UpdatePlayersList_Implementation()
{
	UpdatePlayerListLocal();
}

void ULobbyMenu::UpdatePlayerListLocal()
{
	UWorld* World = GetWorld();
	if (World && World->GetGameState())
	{
		for (int i = 0; i < 4; ++i)
		{
			UTextBlock* TargetTextBlock = nullptr;
			switch (i)
			{
			case 0:
				TargetTextBlock = Text_Red_1;
				break;

			case 1:
				TargetTextBlock = Text_Blue_1;
				break;

			case 2:
				TargetTextBlock = Text_Red_2;
				break;

			case 3:
				TargetTextBlock = Text_Blue_2;
				break;
			}
			if (TargetTextBlock)
			{
				if (i >= World->GetGameState()->PlayerArray.Num())
				{
					TargetTextBlock->SetText(FText::FromString("Waiting for players..."));
				}
				else
				{
					TargetTextBlock->SetText(FText::FromString(World->GetGameState()->PlayerArray[i]->GetPlayerName()));
				}
			}
		}
	}
}

void ULobbyMenu::OnDestroySession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(this, FName("MainMenu"));
	}
}
