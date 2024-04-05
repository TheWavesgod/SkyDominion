// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundOverOverlay.generated.h"

class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class SKYDOMINION_API URoundOverOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitRoundOverOverlay();

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void OnLobbyBttnClicked();

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Result;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_RedScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_BlueScore;

	UPROPERTY(meta = (BindWidget))
	UButton* Bttn_Lobby;
};
