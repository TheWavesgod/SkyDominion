// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerMenu.h"
#include "Components/Button.h"
#include "MultiplayerSessions/Public/MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

bool UMultiplayerMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (Bttn_HostGame)
	{
		Bttn_HostGame->OnClicked.AddDynamic(this, &ThisClass::HostGameClicked);
	}
	if (Bttn_SearchGame)
	{
		Bttn_SearchGame->OnClicked.AddDynamic(this, &ThisClass::SearchGameClicked);
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
	}

	return true;
}

void UMultiplayerMenu::NativeDestruct()
{
	Super::NativeDestruct();
}

void UMultiplayerMenu::JoinSession(FSessionInfo SessionToJoin)
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->JoinSessionByIndex(SessionToJoin.SessionIndex);
	}
}

void UMultiplayerMenu::HostGameClicked()
{
	Bttn_HostGame->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(4, TEXT("FreeForAll"));
	}
}

void UMultiplayerMenu::SearchGameClicked()
{
	Bttn_SearchGame->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(30);
	}
}
 

void UMultiplayerMenu::OnCreateSession(bool bWasSuccessful)
{
	Bttn_HostGame->SetIsEnabled(true);
	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(TEXT("/Game/Maps/MultiplayerLobby?listen"));
		}
	}
	else
	{

	}
}

void UMultiplayerMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		SessionSearchResults.Empty();
		for (int i = 0; i < SessionResults.Num(); ++i)
		{
			FString SettingsValue;
			SessionResults[i].Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
			if (SettingsValue == "FreeForAll")
			{
				FSessionInfo SessionInfo;
				SessionInfo.SessionIndex = i;
				SessionInfo.SessionName = SessionResults[i].Session.OwningUserName;
				SessionInfo.PingInMs = SessionResults[i].PingInMs;
				SessionInfo.VacantNum = SessionResults[i].Session.NumOpenPublicConnections;
				SessionSearchResults.Emplace(SessionInfo);
			}
		}
		UpdateSessionList();
	}
	else
	{
		SessionSearchResults.Empty();
		UpdateSessionList();
	}
	Bttn_SearchGame->SetIsEnabled(true);
}

void UMultiplayerMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
		if (Subsystem)
		{
			IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
			if (SessionInterface.IsValid())
			{
				FString Address;
				SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

				APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
				if (PlayerController)
				{
					PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}
	else
	{

	}
}