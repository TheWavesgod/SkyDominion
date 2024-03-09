// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SkyGameInstance.generated.h"

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

	TArray<int32> PlayersChooseJetList;

	void UpdatePlayersChooseJetList();
};
