// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerState.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerSessions/Public/MultiplayerSessionsSubsystem.h"
#include "SkyDominion/Actor/DisplayAirplane.h"
#include "SkyDominion/SkyFrameWork/SkyGameInstance.h"
#include "SkyDominion/SkyFrameWork/SkyPlayerController.h"

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

	if (ComboBox_FighterJet)
	{
		ComboBox_FighterJet->OnSelectionChanged.AddDynamic(this, &ThisClass::FighterJetSelected);

		UEnum* EnumRef = StaticEnum<EFighterJetType>();
		if (EnumRef)
		{
			for (int i = 0; i < static_cast<int>(EFighterJetType::E_Max); ++i)
			{
				FString EnumName = EnumRef->GetDisplayNameTextByValue(i).ToString(); 
				ComboBox_FighterJet->AddOption(EnumName);
				if (i == 0)
				{
					ComboBox_FighterJet->SetSelectedOption(EnumName);
				}
			}
			
		}
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

	FTimerHandle PlayerListHandle;
	GetWorld()->GetTimerManager().SetTimer(PlayerListHandle, this, &ThisClass::UpdatePlayerListLocal, 0.2f, true);
}

void ULobbyMenu::InitLobby()
{
	bCanUpdatePlayerList = true;

	if (GetWorld()->GetNetMode() == ENetMode::NM_ListenServer)
	{
		Bttn_StartGame->bIsEnabled = true;

		Text_StarttGameBttn->SetText(FText::FromString("Start Game"));
	}
}

void ULobbyMenu::MainMenuBttnClicked()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (World->GetNetMode() != ENetMode::NM_Client)
		{
			MultiplayerSessionsSubsystem->DestroySession();
		}
		else
		{
			UGameplayStatics::OpenLevel(this, FName("MainMenu"));
		}
	}
}

void ULobbyMenu::StartGameBttnClicked()
{
	bCanUpdatePlayerList = false;

	Bttn_StartGame->bIsEnabled = false;

	Text_StarttGameBttn->SetText(FText::FromString("Starting..."));

	USkyGameInstance* GameInstance = GetGameInstance<USkyGameInstance>();
	if (GameInstance)
	{
		GameInstance->UpdatePlayersInfoList();
	}

	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel(TEXT("/Game/Maps/AirCombatTestMap?listen")); 
	}
}

void ULobbyMenu::FighterJetSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	ASkyPlayerState* OwningPlayerState = GEngine->GetFirstLocalPlayerController(GetWorld())->GetPlayerState<ASkyPlayerState>(); 
	if (OwningPlayerState)
	{
		OwningPlayerState->ServerChangeChoosedFighterType(ComboBox_FighterJet->GetSelectedIndex());
	}

	AActor* DisplayActor = UGameplayStatics::GetActorOfClass(this, ADisplayAirplane::StaticClass());
	ADisplayAirplane* DisplayAirplane = Cast<ADisplayAirplane>(DisplayActor);
	if (DisplayAirplane)
	{
		DisplayAirplane->SetDisplayPlaneByIndex(ComboBox_FighterJet->GetSelectedIndex());
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
	if (!bCanUpdatePlayerList) return;

	UWorld* World = GetWorld();

	if (!World) return;

	AGameStateBase* GameState = World->GetGameState();

	if (!GameState) return;

	for (int i = 0; i < 4; ++i)
	{
		UTextBlock* TargetTextBlock = nullptr;

		if (i >= GameState->PlayerArray.Num())
		{
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

			if (TargetTextBlock) TargetTextBlock->SetText(FText::FromString("Waiting for players..."));

			continue;
		}

		ASkyPlayerState* SkyPlayerState = Cast<ASkyPlayerState>(GameState->PlayerArray[i]);

		if (!SkyPlayerState) continue;
		
		int index = SkyPlayerState->bInRedTeam ? 0 : 1;

		index += SkyPlayerState->TeamIndex * 2;

		switch (index)
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
			TargetTextBlock->SetText(FText::FromString(World->GetGameState()->PlayerArray[i]->GetPlayerName()));
		}
	}

	UpdatePlayersFighterTypeLocal();
}

void ULobbyMenu::UpdatePlayersFighterType_Implementation()
{
	UpdatePlayersFighterTypeLocal();
}

void ULobbyMenu::UpdatePlayersFighterTypeLocal()
{
	UEnum* EnumRef = StaticEnum<EFighterJetType>();
	UWorld* World = GetWorld();

	if (!World) return;

	AGameStateBase* GameState = World->GetGameState();

	if (!GameState) return;

	for (int i = 0; i < 4; ++i)
	{
		UTextBlock* TargetTextBlock = nullptr;
		if (i >= GameState->PlayerArray.Num())
		{
			switch (i)
			{
			case 0:
				TargetTextBlock = Text_Red_1_Fighter;
				break;

			case 1:
				TargetTextBlock = Text_Blue_1_Fighter;
				break;

			case 2:
				TargetTextBlock = Text_Red_2_Fighter;
				break;

			case 3:
				TargetTextBlock = Text_Blue_2_Fighter;
				break;
			}

			if (TargetTextBlock) TargetTextBlock->SetText(FText::FromString(""));

			continue;
		}

		ASkyPlayerState* SkyPlayerState = Cast<ASkyPlayerState>(GameState->PlayerArray[i]);

		if (!SkyPlayerState) continue;

		int index = SkyPlayerState->bInRedTeam ? 0 : 1;

		index += SkyPlayerState->TeamIndex * 2;

		switch (index)
		{
		case 0:
			TargetTextBlock = Text_Red_1_Fighter;
			break;

		case 1:
			TargetTextBlock = Text_Blue_1_Fighter;
			break;

		case 2:
			TargetTextBlock = Text_Red_2_Fighter;
			break;

		case 3:
			TargetTextBlock = Text_Blue_2_Fighter;
			break;
		}
		if (TargetTextBlock)
		{
			if (EnumRef)
			{
				TargetTextBlock->SetText(EnumRef->GetDisplayNameTextByValue(int64(SkyPlayerState->ChoosedFighterType)));
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
