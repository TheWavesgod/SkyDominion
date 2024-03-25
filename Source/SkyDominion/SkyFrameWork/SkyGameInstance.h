// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SkyGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FPlayersInfo
{
	GENERATED_BODY()

	bool bInRedTeam;

	int32 TeamIndex;

	int32 ChooseJet;
};

/**
 * 
 */
UCLASS()
class SKYDOMINION_API USkyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	class UMultiplayerMenu* MultiplayerMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	class ULobbyMenu* LobbyMenu;

	UPROPERTY(EditAnywhere, Category = "Game Config")
	TArray<UClass*> FighterJetClass;

	TMap<FUniqueNetIdPtr, FPlayersInfo> PlayersInfoList;

	void UpdatePlayersInfoList();
};
